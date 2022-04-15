//
// Supersingular elliptic curve (Kummer line arithmetic): y² = x³ + Ax² + x
//

#include <string.h>
#include "elliptic_curve.h"
#include "utilities.h"

void x_only_point_copy(x_only_point_t *output, x_only_point_t input) {
    quadratic_field_copy(&output->X, input.X);
    quadratic_field_copy(&output->Z, input.Z);
}

void x_only_point_doubling(x_only_point_t *output,
                           x_only_point_t input_P,
                           projective_curve_alice_t input_curve) {
    quadratic_field_element_t t_0, t_1;

    quadratic_field_subtraction(&t_0, input_P.X, input_P.Z);
    quadratic_field_addition(&t_1, input_P.X, input_P.Z);
    quadratic_field_square(&t_0, t_0);
    quadratic_field_square(&t_1, t_1);
    quadratic_field_multiplication(&output->Z, input_curve.C_times_4, t_0);
    quadratic_field_multiplication(&output->X, output->Z, t_1);
    quadratic_field_subtraction(&t_1, t_1, t_0);
    quadratic_field_multiplication(&t_0, input_curve.A_plus_2C, t_1);
    quadratic_field_addition(&output->Z, output->Z, t_0);
    quadratic_field_multiplication(&output->Z, output->Z, t_1);
}

void x_only_multiple_point_doublings(x_only_point_t *output_2_raised_to_e_P,
                                     x_only_point_t input_P,
                                     uint64_t input_e,
                                     projective_curve_alice_t input_curve) {
    uint64_t i;
    x_only_point_t T;
    x_only_point_copy(&T, input_P);
    for (i = 0; i < input_e; i++) { x_only_point_doubling(&T, T, input_curve); }
    x_only_point_copy(output_2_raised_to_e_P, T);
}

void x_only_point_tripling(x_only_point_t *output,
                           x_only_point_t input_P,
                           projective_curve_bob_t input_curve) {
    quadratic_field_element_t t0, t1, t2, t3, t4;

    quadratic_field_subtraction(&t0, input_P.X, input_P.Z);
    quadratic_field_square(&t2, t0);
    quadratic_field_addition(&t1, input_P.X, input_P.Z);
    quadratic_field_square(&t3, t1);
    quadratic_field_addition(&t4, t1, t0);
    quadratic_field_subtraction(&t0, t1, t0);
    quadratic_field_square(&t1, t4);
    quadratic_field_subtraction(&t1, t1, t3);
    quadratic_field_subtraction(&t1, t1, t2);
    quadratic_field_multiplication(&output->X, t3, input_curve.A_plus_2C);
    quadratic_field_multiplication(&t3, output->X, t3);
    quadratic_field_multiplication(&output->Z, t2, input_curve.A_minus_2C);
    quadratic_field_multiplication(&t2, t2, output->Z);
    quadratic_field_subtraction(&t3, t2, t3);
    quadratic_field_subtraction(&t2, output->X, output->Z);
    quadratic_field_multiplication(&t1, t2, t1);
    quadratic_field_addition(&t2, t3, t1);
    quadratic_field_square(&t2, t2);
    quadratic_field_multiplication(&output->X, t2, t4);
    quadratic_field_subtraction(&t1, t3, t1);
    quadratic_field_square(&t1, t1);
    quadratic_field_multiplication(&output->Z, t1, t0);
}

void x_only_multiple_point_triplings(x_only_point_t *output_3_raised_to_e_P,
                                     x_only_point_t input_P,
                                     uint64_t input_e,
                                     projective_curve_bob_t input_curve) {
    uint64_t i;
    x_only_point_t T;
    x_only_point_copy(&T, input_P);
    for (i = 0; i < input_e; i++) { x_only_point_tripling(&T, T, input_curve); }
    x_only_point_copy(output_3_raised_to_e_P, T);
}

void x_only_point_simultaneous_double_and_addition(x_only_point_t *input_P_output_P2,
                                                   x_only_point_t *input_Q_output_P_plus_Q,
                                                   x_only_point_t input_P_minus_Q,
                                                   quadratic_field_element_t input_quarter_of_A_plus_2) {
    quadratic_field_element_t t0, t1, t2;

    quadratic_field_addition(&t0, input_P_output_P2->X, input_P_output_P2->Z);
    quadratic_field_subtraction(&t1, input_P_output_P2->X, input_P_output_P2->Z);
    quadratic_field_square(&input_P_output_P2->X, t0);
    quadratic_field_subtraction(&t2, input_Q_output_P_plus_Q->X, input_Q_output_P_plus_Q->Z);
    quadratic_field_addition(&input_Q_output_P_plus_Q->X, input_Q_output_P_plus_Q->Z, input_Q_output_P_plus_Q->X);
    quadratic_field_multiplication(&t0, t2, t0);
    quadratic_field_square(&input_P_output_P2->Z, t1);
    quadratic_field_multiplication(&t1, t1, input_Q_output_P_plus_Q->X);
    quadratic_field_subtraction(&t2, input_P_output_P2->X, input_P_output_P2->Z);
    quadratic_field_multiplication(&input_P_output_P2->X, input_P_output_P2->X, input_P_output_P2->Z);
    quadratic_field_multiplication(&input_Q_output_P_plus_Q->X, input_quarter_of_A_plus_2, t2);
    quadratic_field_subtraction(&input_Q_output_P_plus_Q->Z, t0, t1);
    quadratic_field_addition(&input_P_output_P2->Z, input_Q_output_P_plus_Q->X, input_P_output_P2->Z);
    quadratic_field_addition(&input_Q_output_P_plus_Q->X, t0, t1);
    quadratic_field_multiplication(&input_P_output_P2->Z, input_P_output_P2->Z, t2);
    quadratic_field_square(&input_Q_output_P_plus_Q->Z, input_Q_output_P_plus_Q->Z);
    quadratic_field_square(&input_Q_output_P_plus_Q->X, input_Q_output_P_plus_Q->X);
    quadratic_field_multiplication(&input_Q_output_P_plus_Q->Z, input_P_minus_Q.X, input_Q_output_P_plus_Q->Z);
    quadratic_field_multiplication(&input_Q_output_P_plus_Q->X, input_P_minus_Q.Z, input_Q_output_P_plus_Q->X);
}

void x_only_point_swap(x_only_point_t *input_P, x_only_point_t *input_Q, uint64_t input_option) {
    // If option = 0 then P <- P and Q <- Q, else if option = 0xFF...FF then P <- Q and Q <- P
    uint64_t temp;
    unsigned int i;

    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        temp = input_option & (input_P->X.re[i] ^ input_Q->X.re[i]);
        input_P->X.re[i] = temp ^ input_P->X.re[i];
        input_Q->X.re[i] = temp ^ input_Q->X.re[i];
        temp = input_option & (input_P->X.im[i] ^ input_Q->X.im[i]);
        input_P->X.im[i] = temp ^ input_P->X.im[i];
        input_Q->X.im[i] = temp ^ input_Q->X.im[i];
        temp = input_option & (input_P->Z.re[i] ^ input_Q->Z.re[i]);
        input_P->Z.re[i] = temp ^ input_P->Z.re[i];
        input_Q->Z.re[i] = temp ^ input_Q->Z.re[i];
        temp = input_option & (input_P->Z.im[i] ^ input_Q->Z.im[i]);
        input_P->Z.im[i] = temp ^ input_P->Z.im[i];
        input_Q->Z.im[i] = temp ^ input_Q->Z.im[i];
    }
}

void x_only_three_point_ladder(x_only_point_t *output_P_plus_kQ,
                               quadratic_field_element_t input_P,
                               quadratic_field_element_t input_Q,
                               quadratic_field_element_t input_P_minus_Q,
                               const scalar_t input_k,
                               int input_bits_of_k,
                               quadratic_field_element_t input_A) {
    x_only_point_t R0 = {0}, R2 = {0};
    quadratic_field_element_t quarter_of_A_plus_2 = {0};
    uint64_t mask;
    int i;
    uint8_t bit, swap, previous_bit = 0;

    // Initializing constant: quarter_of_A_plus_2 = (A+2)/4
    quadratic_field_set_to_one(&quarter_of_A_plus_2);
    quadratic_field_addition(&quarter_of_A_plus_2, quarter_of_A_plus_2, quarter_of_A_plus_2);
    quadratic_field_addition(&quarter_of_A_plus_2, input_A, quarter_of_A_plus_2);
    quadratic_field_divide_by_2(&quarter_of_A_plus_2, quarter_of_A_plus_2);
    quadratic_field_divide_by_2(&quarter_of_A_plus_2, quarter_of_A_plus_2);

    // Initializing points
    quadratic_field_copy(&R0.X, input_Q);
    quadratic_field_set_to_one(&R0.Z);
    quadratic_field_copy(&R2.X, input_P_minus_Q);
    quadratic_field_set_to_one(&R2.Z);
    quadratic_field_copy(&output_P_plus_kQ->X, input_P);
    quadratic_field_set_to_one(&output_P_plus_kQ->Z);

    // Main loop
    for (i = 0; i < input_bits_of_k; i++) {
        bit = (uint8_t) (input_k[i >> 6] >> (i & (64 - 1))) & 1;
        swap = bit ^ previous_bit;
        previous_bit = bit;
        mask = 0 - (uint64_t) swap;

        x_only_point_swap(output_P_plus_kQ, &R2, mask);
        x_only_point_simultaneous_double_and_addition(&R0,
                                                      &R2,
                                                      *output_P_plus_kQ,
                                                      quarter_of_A_plus_2);
    }
    swap = 0 ^ previous_bit;
    mask = 0 - (uint64_t) swap;
    x_only_point_swap(output_P_plus_kQ, &R2, mask);
}

void to_alice_curve_representation(projective_curve_alice_t *output, projective_curve_bob_t input) {
    quadratic_field_copy(&output->A_plus_2C, input.A_plus_2C);
    quadratic_field_subtraction(&output->C_times_4, input.A_plus_2C, input.A_minus_2C);
}

void to_projective_curve_alice(projective_curve_alice_t *output, quadratic_field_element_t input) {
    quadratic_field_set_to_one(&output->C_times_4);
    quadratic_field_addition(&output->C_times_4, output->C_times_4, output->C_times_4);
    quadratic_field_addition(&output->A_plus_2C, input, output->C_times_4);
    quadratic_field_addition(&output->C_times_4, output->C_times_4, output->C_times_4);
}

void from_projective_curve_alice(quadratic_field_element_t *output, projective_curve_alice_t input) {
    quadratic_field_element_t tmp;
    quadratic_field_inverse(output, input.C_times_4);
    quadratic_field_addition(&tmp, input.A_plus_2C, input.A_plus_2C);
    quadratic_field_subtraction(&tmp, tmp, input.C_times_4);
    quadratic_field_addition(&tmp, tmp, tmp);
    quadratic_field_multiplication(output, tmp, *output);
}

void to_projective_curve_bob(projective_curve_bob_t *output, quadratic_field_element_t input) {
    quadratic_field_set_to_one(&output->A_minus_2C);
    quadratic_field_addition(&output->A_minus_2C, output->A_minus_2C, output->A_minus_2C);
    quadratic_field_addition(&output->A_plus_2C, input, output->A_minus_2C);
    quadratic_field_subtraction(&output->A_minus_2C, input, output->A_minus_2C);
}

void from_projective_curve_bob(quadratic_field_element_t *output, projective_curve_bob_t input) {
    quadratic_field_element_t tmp;
    quadratic_field_subtraction(output, input.A_plus_2C, input.A_minus_2C);
    quadratic_field_inverse(output, *output);
    quadratic_field_addition(&tmp, input.A_plus_2C, input.A_minus_2C);
    quadratic_field_addition(&tmp, tmp, tmp);
    quadratic_field_multiplication(output, tmp, *output);
}

void x_only_point_set_to_infinity(x_only_point_t *input) {
    quadratic_field_set_to_zero(&input->X);
    quadratic_field_set_to_one(&input->Z);
}

uint8_t x_only_point_is_infinity(x_only_point_t P) {
    return quadratic_field_is_zero(P.Z);
}

uint8_t x_only_point_is_equal(x_only_point_t input_P, x_only_point_t input_Q) {
    quadratic_field_element_t s, t;
    quadratic_field_multiplication(&s, input_Q.X, input_P.Z);
    quadratic_field_multiplication(&t, input_Q.Z, input_P.X);
    return quadratic_field_is_equal(s, t);
}

void x_only_point_addition(x_only_point_t *output,
                           x_only_point_t input_P,
                           x_only_point_t input_Q,
                           x_only_point_t input_P_minus_Q) {
    quadratic_field_element_t t0, t1, t2, t3;

    quadratic_field_addition(&t0, input_P.X, input_P.Z);
    quadratic_field_subtraction(&t1, input_Q.X, input_Q.Z);
    quadratic_field_multiplication(&t1, t1, t0);
    quadratic_field_subtraction(&t0, input_P.X, input_P.Z);
    quadratic_field_addition(&t2, input_Q.X, input_Q.Z);
    quadratic_field_multiplication(&t2, t2, t0);
    quadratic_field_addition(&t3, t1, t2);
    quadratic_field_square(&t3, t3);
    quadratic_field_subtraction(&t2, t1, t2);
    quadratic_field_square(&t2, t2);
    quadratic_field_copy(&t1, input_P_minus_Q.X);
    quadratic_field_multiplication(&output->X, input_P_minus_Q.Z, t3);
    quadratic_field_multiplication(&output->Z, t1, t2);
}

void j_invariant(quadratic_field_element_t *output, projective_curve_alice_t input) {
    quadratic_field_element_t t0, t1, a;

    quadratic_field_addition(&a, input.A_plus_2C, input.A_plus_2C);
    quadratic_field_subtraction(&a, a, input.C_times_4);
    quadratic_field_addition(&a, a, a);
    quadratic_field_copy(&t1, input.C_times_4);
    quadratic_field_square(&t1, t1);
    quadratic_field_square(output, a);
    quadratic_field_addition(&t0, t1, t1);
    quadratic_field_subtraction(&t0, *output, t0);
    quadratic_field_subtraction(&t0, t0, t1);
    quadratic_field_subtraction(output, t0, t1);
    quadratic_field_square(&t1, t1);
    quadratic_field_multiplication(output, *output, t1);
    quadratic_field_addition(&t0, t0, t0);
    quadratic_field_addition(&t0, t0, t0);
    quadratic_field_square(&t1, t0);
    quadratic_field_multiplication(&t0, t0, t1);
    quadratic_field_addition(&t0, t0, t0);
    quadratic_field_addition(&t0, t0, t0);
    quadratic_field_inverse(output, *output);
    quadratic_field_multiplication(output, t0, *output);
}

uint8_t x_only_is_full_order_bob(x_only_point_t *P3, x_only_point_t *P, quadratic_field_element_t A) {
    projective_curve_alice_t curve_alice;
    projective_curve_bob_t curve_bob;

    to_projective_curve_alice(&curve_alice, A);
    x_only_multiple_point_doublings(P, *P, EXPONENT_ALICE, curve_alice);

    to_projective_curve_bob(&curve_bob, A);
    x_only_multiple_point_triplings(P3, *P, EXPONENT_BOB - 1, curve_bob);

    return (uint8_t) (x_only_point_is_infinity(*P3) != 1);
}

uint8_t x_only_elligator2(x_only_point_t *P,
                          quadratic_field_element_t u,
                          quadratic_field_element_t r_squared,
                          quadratic_field_element_t A) {
    quadratic_field_element_t one, x_prime, s, t, y;
    prime_field_element_t norm, temp;
    quadratic_field_set_to_one(&one);

    quadratic_field_multiplication(&P->Z, u, r_squared);// ur²
    quadratic_field_multiplication(&x_prime, A, P->Z);  // A×(ur²)
    quadratic_field_multiplication(&y, A, x_prime);     // A²×(ur²)
    quadratic_field_addition(&s, A, x_prime);           // A×(1 + ur²)
    quadratic_field_addition(&P->Z, one, P->Z);         // 1 + ur²
    quadratic_field_square(&t, P->Z);                             // (1 + ur²)²
    quadratic_field_subtraction(&y, y, t);              // A²×(ur²) - (1 + ur²)²
    quadratic_field_multiplication(&y, s, y);           // A×(1 + ur²)×[A²×(ur²) - (1 + ur²)²]
    // We apply the fact y is QR iff norm(y) = (y.re)² + (y.im)² is QR over GF(p)
    prime_field_square(norm, y.re);
    prime_field_square(temp, y.im);
    prime_field_addition(norm, norm, temp);
    // No needed next branch in constant-time (all data is public)
    if (prime_field_is_square(norm)) {
        quadratic_field_negate(&P->X, A);       // -A
        return 1;
    } else {
        quadratic_field_negate(&P->X, x_prime); // -A×(ur²)
        return 0;
    }
}

void x_only_get_P_minus_Q(x_only_point_t *P_minus_Q,
                          quadratic_field_element_t xP,
                          quadratic_field_element_t xQ,
                          quadratic_field_element_t z,
                          quadratic_field_element_t u_prime,
                          quadratic_field_element_t r,
                          quadratic_field_element_t A) {

    quadratic_field_element_t t, s, x;

    quadratic_field_multiplication(&t, u_prime, r);             // u'×r
    prime_field_addition(t.re, t.re, MONTGOMERY_CONSTANT_ONE);  // u'×r + 1
    quadratic_field_square(&t, t);                                       // (u'×r + 1)²

    quadratic_field_square(&P_minus_Q->X, xP);  // X²
    quadratic_field_square(&P_minus_Q->Z, z);   // Z²

    quadratic_field_multiplication(&s, A, P_minus_Q->X);    // A×X²
    quadratic_field_multiplication(&s, s, z);               // A×X²×Z
    quadratic_field_multiplication(&x, xP, P_minus_Q->Z);   // X×Z²
    quadratic_field_addition(&x, s, x);                     // A×X²×Z + X×Z²

    quadratic_field_multiplication(&P_minus_Q->X, P_minus_Q->X, xP);    // X³

    quadratic_field_addition(&x, P_minus_Q->X, x);          // X³ + A×X²×Z + X×Z³
    quadratic_field_multiplication(&P_minus_Q->X, x, t);    // (X³ + A×X²×Z + X×Z³)×(u'×r + 1)²

    quadratic_field_subtraction(&t, xQ, xP);                // X - X'
    quadratic_field_square(&t, t);                                   // (X - X')²
    quadratic_field_multiplication(&P_minus_Q->Z, z, t);    // Z×(X - X')²
}

void x_only_canonical_basis_bob(x_only_point_t *P,
                                x_only_point_t *Q,
                                x_only_point_t *P_minus_Q,
                                quadratic_field_element_t u_prime,
                                quadratic_field_element_t A) {
    uint8_t flag, qr;
    quadratic_field_element_t r_squared, r, u, t;
    x_only_point_t P3, Q3, P3_minus_Q3;

    // The QR r must lie on GF(p²) \ GF(p)
    quadratic_field_set_to_zero(&r);
    prime_field_set_to_one(r.im);
    quadratic_field_square(&u, u_prime);    // we break the rules by assuming u is a QR

    qr = quadratic_field_is_square(&t, A);  // A determines if either QRs or QNRs are required

    for (uint8_t j = 0; j < 32; j++) {
        // +++++++ P
        prime_field_set_to_zero(r.re);
        r.re[0] = TABLE_CANONICAL_BASIS[qr][j];
        prime_field_to_montgomery_representation(r.re, r.re);
        quadratic_field_square(&r_squared, r);
        flag = x_only_elligator2(P, u, r_squared, A);
        if (!flag) { continue; }
        // +++++++ Q
        quadratic_field_multiplication(&Q->X, u, r_squared);
        quadratic_field_multiplication(&Q->X, Q->X, P->X);
        quadratic_field_copy(&Q->Z, P->Z);
        // +++++++ P - Q
        x_only_get_P_minus_Q(P_minus_Q, P->X, Q->X, P->Z, u_prime, r, A);
        // Full order-3ᵇ check
        if (!x_only_is_full_order_bob(&P3, P, A)) { continue; }
        if (!x_only_is_full_order_bob(&Q3, Q, A)) { continue; }
        if (!x_only_is_full_order_bob(&P3_minus_Q3, P_minus_Q, A)) { continue; }
        // Linear independency check by asking [3ᵇ⁻¹]P ≠ ±[3ᵇ⁻¹]Q
        if (x_only_point_is_equal(P3, Q3)) { continue; }
        break;
    }
}

void y_coordinate_recovery_projective(quadratic_field_element_t *yQ,
                                      quadratic_field_element_t *zQ,
                                      quadratic_field_element_t xP,
                                      quadratic_field_element_t yP,
                                      quadratic_field_element_t xQ,
                                      quadratic_field_element_t x_P_minus_Q,
                                      quadratic_field_element_t A) {

    quadratic_field_element_t A2, s, t;

    quadratic_field_multiplication(&t, xP, xQ);
    prime_field_addition(t.re, t.re, MONTGOMERY_CONSTANT_ONE);

    quadratic_field_addition(&A2, A, A);
    quadratic_field_addition(&s, xP, xQ);
    quadratic_field_addition(&s, s, A2);
    quadratic_field_multiplication(&t, t, s);

    quadratic_field_subtraction(&s, xP, xQ);
    quadratic_field_square(&s, s);
    quadratic_field_multiplication(&s, s, x_P_minus_Q);
    quadratic_field_addition(&s, s, A2);
    quadratic_field_subtraction(yQ, s, t);

    quadratic_field_addition(zQ, yP, yP);
}

void x_only_ladder_bob(x_only_point_t *output_kP,
                       x_only_point_t input_P,
                       const scalar_t input_k,
                       int input_bits_of_k,
                       quadratic_field_element_t input_A) {
    int i;
    projective_curve_alice_t curve;
    x_only_point_t x0 = {0}, x1 = {0};

    uint8_t bit;
    to_projective_curve_alice(&curve, input_A);

    // Initializing points
    x_only_point_copy(&x0, input_P);
    x_only_point_doubling(&x1, input_P, curve);

    // Main loop
    for (i = input_bits_of_k - 2; i >= 0; i--) {
        bit = (uint8_t) (input_k[i >> 6] >> (i & (64 - 1))) & 1;
        if (bit == 0) {
            x_only_point_addition(&x1, x0, x1, input_P);
            x_only_point_doubling(&x0, x0, curve);
        } else {
            x_only_point_addition(&x0, x0, x1, input_P);
            x_only_point_doubling(&x1, x1, curve);
        }
    }
    x_only_point_copy(output_kP, x0);
}

void two_dimensional_scalar_multiplication_bob(x_only_point_t *output_cP_plus_dQ,
                                               quadratic_field_element_t input_P,
                                               quadratic_field_element_t input_Q,
                                               quadratic_field_element_t input_P_minus_Q,
                                               const scalar_t input_c,
                                               const scalar_t input_d,
                                               quadratic_field_element_t input_A) {
    scalar_t s0, s1, aux, tmp;
    memset(aux, 0, sizeof(scalar_t));
    x_only_point_t x0, x1, x2, t;
    memcpy(s0, input_c, sizeof(scalar_t));
    memcpy(s1, input_d, sizeof(scalar_t));
    projective_curve_alice_t curve;
    to_projective_curve_alice(&curve, input_A);

    // P
    quadratic_field_copy(&x0.X, input_P);
    quadratic_field_set_to_one(&x0.Z);
    // Q
    quadratic_field_copy(&x1.X, input_Q);
    quadratic_field_set_to_one(&x1.Z);
    // P-Q
    quadratic_field_copy(&x2.X, input_P_minus_Q);
    quadratic_field_set_to_one(&x2.Z);

    while (memcmp(aux, s0, sizeof(scalar_t)) != 0) {
        if (multiprecision_is_smaller(s1, s0, SUBGROUP_ORDER_64BITS_WORDS)) {
            memcpy(tmp, s0, sizeof(scalar_t));
            memcpy(s0, s1, sizeof(scalar_t));
            memcpy(s1, tmp, sizeof(scalar_t));
            x_only_point_copy(&t, x0);
            x_only_point_copy(&x0, x1);
            x_only_point_copy(&x1, t);
        }
        // 4s0
        memcpy(tmp, s0, sizeof(scalar_t));
        multiprecision_shift_to_left(tmp, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_shift_to_left(tmp, SUBGROUP_ORDER_64BITS_WORDS);
        if (!multiprecision_is_smaller(tmp, s1, SUBGROUP_ORDER_64BITS_WORDS)) {
            // s1 <- s1 - s0
            multiprecision_subtraction(s1, s1, s0, SUBGROUP_ORDER_64BITS_WORDS);
            x_only_point_addition(&t, x1, x0, x2);
            x_only_point_copy(&x2, x0);
            x_only_point_copy(&x0, t);
        } else if ((s0[0] & 1) == (s1[0] & 1)) {
            // s1 <- (s1 - s0) / 2
            multiprecision_subtraction(s1, s1, s0, SUBGROUP_ORDER_64BITS_WORDS);
            multiprecision_shift_to_right(s1, SUBGROUP_ORDER_64BITS_WORDS);
            x_only_point_addition(&x0, x1, x0, x2);
            x_only_point_doubling(&x1, x1, curve);
        } else if ((s1[0] & 1) == 0) {
            // s1 <- s1 / 2
            multiprecision_shift_to_right(s1, SUBGROUP_ORDER_64BITS_WORDS);
            x_only_point_addition(&x2, x1, x2, x0);
            x_only_point_doubling(&x1, x1, curve);
        } else {
            // s0 <- s0 / 2
            multiprecision_shift_to_right(s0, SUBGROUP_ORDER_64BITS_WORDS);
            x_only_point_addition(&x2, x0, x2, x1);
            x_only_point_doubling(&x0, x0, curve);
        }
    }
    // Small common factor between c and d;
    while ((s1[0] & 1) == 0) {
        // s1 <- s1 / 2
        multiprecision_shift_to_right(s1, SUBGROUP_ORDER_64BITS_WORDS);
        x_only_point_doubling(&x1, x1, curve);
    }
    // Get bit-length of s1
    aux[0] = 1;
    uint64_t j, flag, stop = 0;
    int i, bits = SUBGROUP_ORDER_64BITS_WORDS * 64;
    for (i = SUBGROUP_ORDER_64BITS_WORDS - 1; i >= 0; i--) {
        flag = 0x8000000000000000;
        for (j = 0; j < 64; j++) {
            if ((s1[i] & flag) != 0) {
                stop = 1;
                break;
            }
            bits -= 1;
            flag >>= 1;
        }
        if (stop) { break; }
    }
    if (multiprecision_is_smaller(aux, s1, SUBGROUP_ORDER_64BITS_WORDS)) {
        x_only_ladder_bob(&x1, x1, s1, bits, input_A);
    }
    x_only_point_copy(output_cP_plus_dQ, x1);
}

uint8_t x_only_point_is_equal_mixed(x_only_point_t input_P, quadratic_field_element_t input_xP) {
    quadratic_field_element_t t;
    quadratic_field_multiplication(&t, input_xP, input_P.Z);
    return quadratic_field_is_equal(input_P.X, t);
}

uint8_t projective_curve_is_equal_mixed_alice(projective_curve_alice_t input_curve, quadratic_field_element_t input_A) {
    quadratic_field_element_t t0, t1;
    quadratic_field_addition(&t0, input_curve.A_plus_2C, input_curve.A_plus_2C);
    quadratic_field_subtraction(&t0, t0, input_curve.C_times_4);
    quadratic_field_addition(&t0, t0, t0);                                              // 4A
    quadratic_field_multiplication(&t1, input_curve.C_times_4, input_A);
    return quadratic_field_is_equal(t0, t1);
}

uint8_t projective_curve_is_equal_mixed_bob(projective_curve_bob_t input_curve, quadratic_field_element_t input_A) {
    quadratic_field_element_t t0, t1;
    quadratic_field_subtraction(&t0, input_curve.A_plus_2C, input_curve.A_minus_2C);    // 4C
    quadratic_field_addition(&t1, input_curve.A_plus_2C, input_curve.A_minus_2C);
    quadratic_field_addition(&t1, t1, t1);                                              // 4A
    quadratic_field_multiplication(&t0, t0, input_A);
    return quadratic_field_is_equal(t0, t1);
}
