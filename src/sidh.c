//
// SIDH built-in functions
//

#include <string.h>
#include "sidh.h"

void simultaneous_three_inverses(quadratic_field_element_t *input_output_z1,
                                 quadratic_field_element_t *input_output_z2,
                                 quadratic_field_element_t *input_output_z3) {
    quadratic_field_element_t t0, t1, t2;

    quadratic_field_multiplication(&t0, *input_output_z1, *input_output_z2);
    quadratic_field_multiplication(&t1, *input_output_z3, t0);
    quadratic_field_inverse(&t1, t1);
    quadratic_field_multiplication(&t2, *input_output_z3, t1);
    quadratic_field_multiplication(input_output_z3, t0, t1);
    quadratic_field_multiplication(&t0, t2, *input_output_z2);
    quadratic_field_multiplication(input_output_z2, t2, *input_output_z1);
    quadratic_field_copy(input_output_z1, t0);
}

void get_curve_from_points(quadratic_field_element_t *output,
                           quadratic_field_element_t input_P,
                           quadratic_field_element_t input_Q,
                           quadratic_field_element_t input_P_minus_Q) {
    quadratic_field_element_t t0, t1, one;
    quadratic_field_set_to_one(&one);

    quadratic_field_addition(&t1, input_P, input_Q);
    quadratic_field_multiplication(&t0, input_P, input_Q);
    quadratic_field_multiplication(output, input_P_minus_Q, t1);
    quadratic_field_addition(output, t0, *output);
    quadratic_field_multiplication(&t0, t0, input_P_minus_Q);
    quadratic_field_subtraction(output, *output, one);
    quadratic_field_addition(&t0, t0, t0);
    quadratic_field_addition(&t1, t1, input_P_minus_Q);
    quadratic_field_addition(&t0, t0, t0);
    quadratic_field_square(output, *output);
    quadratic_field_inverse(&t0, t0);
    quadratic_field_multiplication(output, *output, t0);
    quadratic_field_subtraction(output, *output, t1);
}

// +++++++ ALICE
void sidh_generate_private_key_alice(sidh_private_key_t *private_key_alice, keccak_state *state) {
    shake256_squeeze((uint8_t *) private_key_alice, SECRET_KEY_BYTES_ALICE, state);
    private_key_alice->key[SUBGROUP_ORDER_64BITS_WORDS - 1] &= MASK_ALICE_64BITS;
}

void sidh_get_public_key_from_private_key_alice(sidh_public_key_t *public_key_alice,
                                                const sidh_private_key_t *private_key_alice) {

    quadratic_field_element_t a, t;
    quadratic_field_set_to_one(&a);                     // 1
    quadratic_field_addition(&t, a, a); // 2
    quadratic_field_addition(&a, t, a); // 3
    quadratic_field_addition(&a, a, a); // 6

    projective_curve_alice_t curve;
    to_projective_curve_alice(&curve, a);

    // +++++++ order-2ᵃ point generators (secret kernel point computation)
    x_only_point_t P = {0}, Q = {0}, P_minus_Q = {0}, K = {0};
    initialize_basis(&P.X, &Q.X, &P_minus_Q.X, PUBLIC_POINT_GENERATORS_ALICE);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_set_to_one(&P_minus_Q.Z);
    x_only_three_point_ladder(&K,
                              P.X,
                              Q.X,
                              P_minus_Q.X,
                              private_key_alice->key,
                              SUBGROUP_ORDER_BITS_ALICE,
                              a);

    // +++++++ order-3ᵇ point generators (public key)
    initialize_basis(&P.X, &Q.X, &P_minus_Q.X, PUBLIC_POINT_GENERATORS_BOB);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_set_to_one(&P_minus_Q.Z);

    x_only_isogeny_from_kernel_alice(&P, &Q, &P_minus_Q, &curve, K);

    simultaneous_three_inverses(&P.Z, &Q.Z, &P_minus_Q.Z);
    quadratic_field_multiplication(&public_key_alice->P, P.X, P.Z);
    quadratic_field_multiplication(&public_key_alice->Q, Q.X, Q.Z);
    quadratic_field_multiplication(&public_key_alice->P_minus_Q, P_minus_Q.X, P_minus_Q.Z);
}

void sidh_key_agreement_alice(sidh_shared_secret_t *shared_secret_alice,
                              const sidh_private_key_t *private_key_alice,
                              const sidh_public_key_t *public_key_bob) {
    quadratic_field_element_t a = {0};

    projective_curve_alice_t curve;
    get_curve_from_points(&a,
                          public_key_bob->P,
                          public_key_bob->Q,
                          public_key_bob->P_minus_Q);
    to_projective_curve_alice(&curve, a);

    // +++++++ order-2ᵃ point generators (secret kernel point computation)
    x_only_point_t P = {0}, Q = {0}, P_minus_Q = {0}, K = {0};
    quadratic_field_copy(&P.X, public_key_bob->P);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_copy(&Q.X, public_key_bob->Q);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_copy(&P_minus_Q.X, public_key_bob->P_minus_Q);
    quadratic_field_set_to_one(&P_minus_Q.Z);
    x_only_three_point_ladder(&K,
                              P.X,
                              Q.X,
                              P_minus_Q.X,
                              private_key_alice->key,
                              SUBGROUP_ORDER_BITS_ALICE,
                              a);

    x_only_isogeny_from_kernel_curve_alice(&curve, K, curve);
    from_projective_curve_alice(&shared_secret_alice->curve, curve);
}

// +++++++ BOB
void sidh_generate_private_key_bob(sidh_private_key_t *private_key_bob, keccak_state *state) {
    shake256_squeeze((uint8_t *) private_key_bob, SECRET_KEY_BYTES_BOB, state);
    private_key_bob->key[SUBGROUP_ORDER_64BITS_WORDS - 1] &= MASK_BOB_64BITS;
}

void sidh_get_public_key_from_private_key_bob(sidh_public_key_t *public_key_bob,
                                              const sidh_private_key_t *private_key_bob) {

    quadratic_field_element_t a, t;
    quadratic_field_set_to_one(&a);                     // 1
    quadratic_field_addition(&t, a, a); // 2
    quadratic_field_addition(&a, t, a); // 3
    quadratic_field_addition(&a, a, a); // 6

    projective_curve_bob_t curve;
    to_projective_curve_bob(&curve, a);

    // +++++++ order-3ᵇ point generators (secret kernel point computation)
    x_only_point_t P = {0}, Q = {0}, P_minus_Q = {0}, K = {0};
    initialize_basis(&P.X, &Q.X, &P_minus_Q.X, PUBLIC_POINT_GENERATORS_BOB);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_set_to_one(&P_minus_Q.Z);
    x_only_three_point_ladder(&K,
                              P.X,
                              Q.X,
                              P_minus_Q.X,
                              private_key_bob->key,
                              SUBGROUP_ORDER_BITS_BOB,
                              a);

    // +++++++ order-2ᵃ point generators (public key)
    initialize_basis(&P.X, &Q.X, &P_minus_Q.X, PUBLIC_POINT_GENERATORS_ALICE);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_set_to_one(&P_minus_Q.Z);

    x_only_isogeny_from_kernel_bob(&P, &Q, &P_minus_Q, &curve, K);

    simultaneous_three_inverses(&P.Z, &Q.Z, &P_minus_Q.Z);
    quadratic_field_multiplication(&public_key_bob->P, P.X, P.Z);
    quadratic_field_multiplication(&public_key_bob->Q, Q.X, Q.Z);
    quadratic_field_multiplication(&public_key_bob->P_minus_Q, P_minus_Q.X, P_minus_Q.Z);
}

void sidh_key_agreement_bob(sidh_shared_secret_t *shared_secret_bob,
                            const sidh_private_key_t *private_key_bob,
                            const sidh_public_key_t *public_key_alice) {
    quadratic_field_element_t a = {0};

    projective_curve_bob_t curve;
    get_curve_from_points(&a,
                          public_key_alice->P,
                          public_key_alice->Q,
                          public_key_alice->P_minus_Q);
    to_projective_curve_bob(&curve, a);

    // +++++++ order-2ᵃ point generators (secret kernel point computation)
    x_only_point_t P = {0}, Q = {0}, P_minus_Q = {0}, K = {0};
    quadratic_field_copy(&P.X, public_key_alice->P);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_copy(&Q.X, public_key_alice->Q);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_copy(&P_minus_Q.X, public_key_alice->P_minus_Q);
    quadratic_field_set_to_one(&P_minus_Q.Z);
    x_only_three_point_ladder(&K,
                              P.X,
                              Q.X,
                              P_minus_Q.X,
                              private_key_bob->key,
                              SUBGROUP_ORDER_BITS_BOB,
                              a);

    x_only_isogeny_from_kernel_curve_bob(&curve, K, curve);
    from_projective_curve_bob(&shared_secret_bob->curve, curve);
}

void sidh_public_key_to_bytes(uint8_t pk_bytes[PUBLIC_KEY_BYTES], const sidh_public_key_t *pk) {
    quadratic_field_element_to_bytes(pk_bytes, pk->P);
    quadratic_field_element_to_bytes(&pk_bytes[QUADRATIC_FIELD_BYTES], pk->Q);
    quadratic_field_element_to_bytes(&pk_bytes[2 * QUADRATIC_FIELD_BYTES], pk->P_minus_Q);
}

void sidh_public_key_from_bytes(sidh_public_key_t *pk, const uint8_t pk_bytes[PUBLIC_KEY_BYTES]) {
    quadratic_field_element_from_bytes(&pk->P, pk_bytes);
    quadratic_field_element_from_bytes(&pk->Q, &pk_bytes[QUADRATIC_FIELD_BYTES]);
    quadratic_field_element_from_bytes(&pk->P_minus_Q, &pk_bytes[2 * QUADRATIC_FIELD_BYTES]);
}

inline void sidh_private_key_to_bytes_bob(uint8_t sk_bytes[44], const sidh_private_key_t *sk) {
    memcpy(sk_bytes, sk->key, SECRET_KEY_BYTES_BOB);
}

inline void sidh_private_key_from_bytes_bob(sidh_private_key_t *sk, const uint8_t sk_bytes[44]) {
    memcpy(sk->key, sk_bytes, SECRET_KEY_BYTES_BOB);
}

inline void sidh_private_key_to_bytes_alice(uint8_t sk_bytes[PUBLIC_KEY_BYTES], const sidh_private_key_t *sk) {
    memcpy(sk_bytes, sk->key, SECRET_KEY_BYTES_ALICE);
}

inline void sidh_private_key_from_bytes_alice(sidh_private_key_t *sk, const uint8_t sk_bytes[PUBLIC_KEY_BYTES]) {
    memcpy(sk->key, sk_bytes, SECRET_KEY_BYTES_ALICE);
}
