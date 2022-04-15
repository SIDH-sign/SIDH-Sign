//
// Proof Of Knowledge (PoK)
// SIDH built-in functions
//

#include <stdlib.h>
#include <string.h>
#include "pok.h"
#include "utilities.h"

// Most Significant Byte determines the shape of the kernel: either P + [t3ᵇ⁻¹ + k]Q or [3k]P + Q where t in {0,1,2}
void random_private_key_sample(uint8_t output[SECRET_KEY_BYTES_BOB + 1], keccak_state *state) {
    // We should move computation of max into a global variable
    uint64_t max[SUBGROUP_ORDER_64BITS_WORDS] = {0}, t[SUBGROUP_ORDER_64BITS_WORDS];
    max[0] = 1;
    for (int i = 1; i < EXPONENT_BOB; i++) {
        memcpy(t, max, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_shift_to_left(max, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(max, max, t, SUBGROUP_ORDER_64BITS_WORDS);
    }

    shake256_squeeze((uint8_t *) output, SECRET_KEY_BYTES_BOB + 1, state);
    output[SECRET_KEY_BYTES_BOB - 1] &= MASK_BOB;
    output[SECRET_KEY_BYTES_BOB] &= 0x03;
    memset(t, 0, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
    memcpy(t, output, SECRET_KEY_BYTES_BOB);
    while (!multiprecision_is_smaller(t, max, SUBGROUP_ORDER_64BITS_WORDS)) {
        shake256_squeeze((uint8_t *) output, SECRET_KEY_BYTES_BOB + 1, state);
        output[SECRET_KEY_BYTES_BOB - 1] &= MASK_BOB;
        output[SECRET_KEY_BYTES_BOB] &= 0x03;
        memcpy(t, output, SECRET_KEY_BYTES_BOB);
    }
}

void canonical_basis(sidh_public_key_t *output_basis, quadratic_field_element_t input_A) {
    quadratic_field_element_t u;
    x_only_point_t P, Q, P_minus_Q;

    // u = 1 + i
    prime_field_set_to_one(u.re);
    prime_field_set_to_one(u.im);

    x_only_canonical_basis_bob(&P, &Q, &P_minus_Q, u, input_A);

    simultaneous_three_inverses(&P.Z, &Q.Z, &P_minus_Q.Z);
    quadratic_field_multiplication(&output_basis->P, P.X, P.Z);
    quadratic_field_multiplication(&output_basis->Q, Q.X, Q.Z);
    quadratic_field_multiplication(&output_basis->P_minus_Q, P_minus_Q.X, P_minus_Q.Z);
}

uint8_t decomposition_by_scalars(scalar_t output_c,
                                 scalar_t output_d,
                                 quadratic_field_element_t input_kernel,
                                 sidh_public_key_t input_basis,
                                 quadratic_field_element_t input_A) {
    uint8_t ret;
    quadratic_field_element_t yP, yQ, zQ, t, yK;

    projective_point_t P_twisted_edwards, Q_twisted_edwards, K_twisted_edwards;
    twisted_edwards_curve_t curve;

    // +++++++ y-coordinate of P
    // square-root of x³ + Ax² + x
    quadratic_field_square(&t, input_basis.P);
    quadratic_field_multiplication(&yP, input_A, t);
    quadratic_field_addition(&yP, yP, input_basis.P);
    quadratic_field_multiplication(&t, t, input_basis.P);
    quadratic_field_addition(&yP, yP, t);
    ret = quadratic_field_is_square(&yP, yP);
    if (!ret) { return EXIT_FAILURE; }

    // +++++++  y-coordinate of Q as (Y:Z) such that y=Y/Z
    y_coordinate_recovery_projective(&yQ,
                                     &zQ,
                                     input_basis.P,
                                     yP,
                                     input_basis.Q,
                                     input_basis.P_minus_Q,
                                     input_A);

    // +++++++  y-coordinate of the kernel
    // square-root of x³ + Ax² + x
    quadratic_field_square(&t, input_kernel);
    quadratic_field_multiplication(&yK, input_A, t);
    quadratic_field_addition(&yK, yK, input_kernel);
    quadratic_field_multiplication(&t, t, input_kernel);
    quadratic_field_addition(&yK, yK, t);
    ret = quadratic_field_is_square(&yK, yK);
    if (!ret) { return EXIT_FAILURE; }

    to_twisted_edwards_model(&P_twisted_edwards, input_basis.P, yP);
    to_twisted_edwards_model_mixed(&Q_twisted_edwards, input_basis.Q, yQ, zQ);
    to_twisted_edwards_model(&K_twisted_edwards, input_kernel, yK);

    // To Twisted Edwards model by setting a = A + 2 and d = A - 2
    quadratic_field_copy(&curve.a, input_A);
    prime_field_addition(curve.a.re, curve.a.re, MONTGOMERY_CONSTANT_ONE);
    prime_field_addition(curve.a.re, curve.a.re, MONTGOMERY_CONSTANT_ONE);
    quadratic_field_copy(&curve.d, input_A);
    prime_field_subtraction(curve.d.re, curve.d.re, MONTGOMERY_CONSTANT_ONE);
    prime_field_subtraction(curve.d.re, curve.d.re, MONTGOMERY_CONSTANT_ONE);

    two_dimensional_pohlig_hellman_bob(output_c,
                                       output_d,
                                       K_twisted_edwards,
                                       P_twisted_edwards,
                                       Q_twisted_edwards,
                                       curve);
    return EXIT_SUCCESS;
}


void sidh_pok_commitment(uint8_t commit[COMMITMENT_BYTES],
                         uint8_t insight[INSIGHT_BYTES],
                         const uint8_t private_key_alice[SECRET_KEY_BYTES_ALICE],
                         keccak_state *state) {
    sidh_private_key_t private_key, ephemeral_key, ephemeral_key_3;
    x_only_point_t P0, Q0, P0_minus_Q0, // Public points: either {PA, QA, PA-QA} or {PB, QB, PB-QB}
    K_phi, K_psi, K_psi_dual;           // kernel generators
    quadratic_field_element_t A, u, z;
    projective_curve_alice_t curve_alice;
    projective_curve_bob_t curve_bob;
    sidh_public_key_t public_key;
    uint8_t ephemeral_key_bytes[SECRET_KEY_BYTES_BOB + 1];

    private_key.key[SUBGROUP_ORDER_64BITS_WORDS - 1] = 0;
    memcpy(private_key.key, private_key_alice, SECRET_KEY_BYTES_ALICE);

    random_private_key_sample(ephemeral_key_bytes, state);
    memcpy(ephemeral_key.key, ephemeral_key_bytes, SECRET_KEY_BYTES_BOB);

    initialize_basis(&P0.X, &Q0.X, &P0_minus_Q0.X, PUBLIC_POINT_GENERATORS_BOB);
    quadratic_field_set_to_one(&P0.Z);
    quadratic_field_set_to_one(&Q0.Z);
    quadratic_field_set_to_one(&P0_minus_Q0.Z);

    // 3×ephemeral_key
    memcpy(ephemeral_key_3.key, ephemeral_key_bytes, SECRET_KEY_BYTES_BOB);
    multiprecision_shift_to_left(ephemeral_key_3.key, SUBGROUP_ORDER_64BITS_WORDS);
    multiprecision_addition(ephemeral_key_3.key,
                            ephemeral_key_3.key,
                            ephemeral_key.key,
                            SUBGROUP_ORDER_64BITS_WORDS);

    // +++++++ Look at this, we also need it here (we need to move it as a global variable?)
    uint64_t max[SUBGROUP_ORDER_64BITS_WORDS] = {0}, t[SUBGROUP_ORDER_64BITS_WORDS];
    max[0] = 1;
    for (int i = 1; i < EXPONENT_BOB; i++) {
        memcpy(t, max, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_shift_to_left(max, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(max, max, t, SUBGROUP_ORDER_64BITS_WORDS);
    }
    // Linear pass to get the right scalar and swapping of points [CHECK THE NEXT CONDITIONAL MOVS]
    // 3ᵇ⁻¹ + k
    multiprecision_addition(t, max, ephemeral_key.key, SUBGROUP_ORDER_64BITS_WORDS);
    constant_time_conditional_mov((uint8_t *) ephemeral_key.key,
                                  (uint8_t *) t,
                                  SECRET_KEY_BYTES_BOB,
                                  0 - (uint8_t) (ephemeral_key_bytes[SECRET_KEY_BYTES_BOB] == 0x01));
    // 2×3ᵇ⁻¹ + k
    multiprecision_addition(t, max, t, SUBGROUP_ORDER_64BITS_WORDS);
    constant_time_conditional_mov((uint8_t *) ephemeral_key.key,
                                  (uint8_t *) t,
                                  SECRET_KEY_BYTES_BOB,
                                  0 - (uint8_t) (ephemeral_key_bytes[SECRET_KEY_BYTES_BOB] == 0x02));
    // 3×k
    constant_time_conditional_mov((uint8_t *) ephemeral_key.key,
                                  (uint8_t *) ephemeral_key_3.key,
                                  SECRET_KEY_BYTES_BOB,
                                  0 - (uint8_t) (ephemeral_key_bytes[SECRET_KEY_BYTES_BOB] == 0x03));
    // Swap points if needed
    x_only_point_swap(&P0,
                      &Q0,
                      0 - (uint64_t) (ephemeral_key_bytes[SECRET_KEY_BYTES_BOB] == 0x03));
    x_only_point_copy(&K_psi_dual, Q0);
    // Base public curve y² = x³ + x (that is, A=0)... [Do we need to make it randomly the initial curve?]
    quadratic_field_set_to_one(&A);                     // 1
    quadratic_field_addition(&u, A, A); // 2
    quadratic_field_addition(&A, u, A); // 3
    quadratic_field_addition(&A, A, A); // 6
    // order-3ᵇ kernel point generator of ker ψ (secret kernel point computation)
    x_only_three_point_ladder(&K_psi,
                              P0.X,
                              Q0.X,
                              P0_minus_Q0.X,
                              ephemeral_key.key,
                              SUBGROUP_ORDER_BITS_BOB,
                              A);
    // order-2ᵃ kernel point generator of ker φ (secret kernel point computation)
    initialize_basis(&P0.X, &Q0.X, &P0_minus_Q0.X, PUBLIC_POINT_GENERATORS_ALICE);
    quadratic_field_set_to_one(&P0.Z);
    quadratic_field_set_to_one(&Q0.Z);
    quadratic_field_set_to_one(&P0_minus_Q0.Z);
    x_only_three_point_ladder(&K_phi,
                              P0.X,
                              Q0.X,
                              P0_minus_Q0.X,
                              private_key.key,
                              SUBGROUP_ORDER_BITS_ALICE,
                              A);
    // Computation of E₂ and ker φ' = ψ (ker φ)
    to_projective_curve_bob(&curve_bob, A);
    x_only_isogeny_from_kernel_point_and_curve_bob(&K_phi,
                                                   &K_psi_dual,
                                                   &curve_bob,
                                                   K_psi);

    quadratic_field_multiplication(&u, K_phi.Z, K_psi_dual.Z);
    quadratic_field_inverse(&u, u);
    quadratic_field_multiplication(&z, K_psi_dual.Z, u);
    quadratic_field_multiplication(&z, K_phi.X, z);
    quadratic_field_element_to_bytes(insight, z);                            // ker φ
    quadratic_field_multiplication(&z, K_phi.Z, u);
    quadratic_field_multiplication(&z, K_psi_dual.X, z);
    quadratic_field_element_to_bytes(&insight[QUADRATIC_FIELD_BYTES], z);    // ker (dual of ψ)

    from_projective_curve_bob(&A, curve_bob);
    quadratic_field_element_to_bytes(commit, A);
    quadratic_field_element_to_bytes(&insight[2 * QUADRATIC_FIELD_BYTES], A);
    // Looking for P₂, Q₂, and (P₂ - Q₂)
    canonical_basis(&public_key, A);
    sidh_public_key_to_bytes(&insight[3 * QUADRATIC_FIELD_BYTES], &public_key);
    quadratic_field_copy(&P0.X, public_key.P);
    quadratic_field_set_to_one(&P0.Z);
    quadratic_field_copy(&Q0.X, public_key.Q);
    quadratic_field_set_to_one(&Q0.Z);
    quadratic_field_copy(&P0_minus_Q0.X, public_key.P_minus_Q);
    quadratic_field_set_to_one(&P0_minus_Q0.Z);
    // Computing P₃ = φ'(P₂), Q₃ = φ'(Q₂), and (P₃-Q₃) = φ'(P₂ - Q₂)
    to_projective_curve_alice(&curve_alice, A);
    x_only_isogeny_from_kernel_alice(&P0,
                                     &Q0,
                                     &P0_minus_Q0,
                                     &curve_alice,
                                     K_phi);
    simultaneous_three_inverses(&P0.Z, &Q0.Z, &P0_minus_Q0.Z);
    quadratic_field_multiplication(&public_key.P, P0.X, P0.Z);
    quadratic_field_multiplication(&public_key.Q, Q0.X, Q0.Z);
    quadratic_field_multiplication(&public_key.P_minus_Q, P0_minus_Q0.X, P0_minus_Q0.Z);
    sidh_public_key_to_bytes(&commit[QUADRATIC_FIELD_BYTES], &public_key);
}

void sidh_pok_challenge(uint8_t *challenge, const uint8_t *commitment, uint64_t commitment_size,
                        const uint8_t *statement, uint64_t statement_size) {
    uint8_t separator = 0xFF;
    keccak_state keccak_st;
    shake256_init(&keccak_st);
    shake256_absorb(&keccak_st, commitment, commitment_size);
    shake256_absorb(&keccak_st, &separator, 1);
    shake256_absorb(&keccak_st, statement, statement_size);

    shake256_squeeze(challenge, SECURITY_BITS / 8, &keccak_st);
    shake256_finalize(&keccak_st);
}


uint8_t sidh_pok_response(uint8_t resp[RESPONSE_BYTES], const uint8_t insight[INSIGHT_BYTES], uint8_t challenge) {
    memset(resp, 0, RESPONSE_BYTES);
    if (!challenge) {
        // challenge = 0
        quadratic_field_element_t K_psi_dual, A;
        sidh_public_key_t basis;
        quadratic_field_element_from_bytes(&K_psi_dual, &insight[QUADRATIC_FIELD_BYTES]);
        quadratic_field_element_from_bytes(&A, &insight[2 * QUADRATIC_FIELD_BYTES]);
        sidh_public_key_from_bytes(&basis, &insight[3 * QUADRATIC_FIELD_BYTES]);
        scalar_t c = {0}, d = {0};
        uint8_t success = decomposition_by_scalars(c,
                                                   d,
                                                   K_psi_dual,
                                                   basis,
                                                   A);
        if (success == EXIT_FAILURE) { return EXIT_FAILURE; }
        memcpy(resp, (const uint8_t *) c, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
        memcpy(&resp[FIELD_BYTES], (const uint8_t *) d, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
        return EXIT_SUCCESS;
    } else {
        memcpy(resp, insight, RESPONSE_BYTES);
        return EXIT_SUCCESS;
    }
}

uint8_t sidh_pok_verification(const uint8_t commit[COMMITMENT_BYTES],
                              const uint8_t resp[RESPONSE_BYTES],
                              const uint8_t public_key_alice[PUBLIC_KEY_BYTES],
                              uint8_t challenge) {
    sidh_public_key_t basis1, basis2, basis3;
    quadratic_field_element_t E0, E1, E2, E3, t;

    quadratic_field_set_to_one(&E0);                        // 1
    quadratic_field_addition(&t, E0, E0);   // 2
    quadratic_field_addition(&E0, E0, t);   // 3
    quadratic_field_addition(&E0, E0, E0);  // 6

    sidh_public_key_from_bytes(&basis1, public_key_alice);
    get_curve_from_points(&E1, basis1.P, basis1.Q, basis1.P_minus_Q);

    quadratic_field_element_from_bytes(&E2, commit);
    canonical_basis(&basis2, E2);

    sidh_public_key_from_bytes(&basis3, &commit[QUADRATIC_FIELD_BYTES]);
    get_curve_from_points(&E3, basis3.P, basis3.Q, basis3.P_minus_Q);
    if (!challenge) {
        // challenge = 0
        scalar_t c = {0}, d = {0};
        x_only_point_t K_psi_dual, K_psi_prime_dual, T, R;
        projective_curve_bob_t curve2, curve3;
        memcpy((uint8_t *) c, resp, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
        memcpy((uint8_t *) d, &resp[FIELD_BYTES], sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
        two_dimensional_scalar_multiplication_bob(&K_psi_dual,
                                                  basis2.P,
                                                  basis2.Q,
                                                  basis2.P_minus_Q,
                                                  c,
                                                  d,
                                                  E2);
        to_projective_curve_bob(&curve2, E2);
        x_only_multiple_point_triplings(&T, K_psi_dual, EXPONENT_BOB - 1, curve2);
        x_only_point_tripling(&R, T, curve2);
        if (!x_only_point_is_infinity(R) || x_only_point_is_infinity(T)) { return EXIT_FAILURE; }    // full-order?

        two_dimensional_scalar_multiplication_bob(&K_psi_prime_dual,
                                                  basis3.P,
                                                  basis3.Q,
                                                  basis3.P_minus_Q,
                                                  c,
                                                  d,
                                                  E3);
        to_projective_curve_bob(&curve3, E3);
        x_only_multiple_point_triplings(&T, K_psi_prime_dual, EXPONENT_BOB - 1, curve3);
        x_only_point_tripling(&R, T, curve3);
        if (!x_only_point_is_infinity(R) || x_only_point_is_infinity(T)) { return EXIT_FAILURE; }    // full-order?

        x_only_isogeny_from_kernel_curve_bob(&curve2, K_psi_dual, curve2);
        if (!projective_curve_is_equal_mixed_bob(curve2, E0)) { return EXIT_FAILURE; }
        x_only_isogeny_from_kernel_curve_bob(&curve3, K_psi_prime_dual, curve3);
        if (!projective_curve_is_equal_mixed_bob(curve3, E1)) { return EXIT_FAILURE; }
    } else {
        x_only_point_t K_phi_prime, T, R, R_minus_T;
        projective_curve_alice_t curve2;
        to_projective_curve_alice(&curve2, E2);
        quadratic_field_element_from_bytes(&K_phi_prime.X, resp);
        quadratic_field_set_to_one(&K_phi_prime.Z);

        x_only_multiple_point_doublings(&T, K_phi_prime, EXPONENT_ALICE - 1, curve2);
        x_only_point_doubling(&R, T, curve2);
        if (!x_only_point_is_infinity(R) || x_only_point_is_infinity(T)) { return EXIT_FAILURE; }    // full-order?

        quadratic_field_copy(&R.X, basis2.P);
        quadratic_field_set_to_one(&R.Z);
        quadratic_field_copy(&T.X, basis2.Q);
        quadratic_field_set_to_one(&T.Z);
        quadratic_field_copy(&R_minus_T.X, basis2.P_minus_Q);
        quadratic_field_set_to_one(&R_minus_T.Z);
        x_only_isogeny_from_kernel_alice(&R,
                                         &T,
                                         &R_minus_T,
                                         &curve2,
                                         K_phi_prime);
        if (!projective_curve_is_equal_mixed_alice(curve2, E3)) { return EXIT_FAILURE; }
        if (!x_only_point_is_equal_mixed(R, basis3.P)) { return EXIT_FAILURE; }
        if (!x_only_point_is_equal_mixed(T, basis3.Q)) { return EXIT_FAILURE; }
        if (!x_only_point_is_equal_mixed(R_minus_T, basis3.P_minus_Q)) { return EXIT_FAILURE; }
    }
    return EXIT_SUCCESS;
}
