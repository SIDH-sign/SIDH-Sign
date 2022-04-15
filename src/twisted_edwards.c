//
//  Twisted Edwards curve: ax² + y² = 1 + dx²y²
//

#include "twisted_edwards.h"
#include "utilities.h"
#include <string.h>

void projective_point_addition(projective_point_t *output,
                               projective_point_t input_P,
                               projective_point_t input_Q,
                               twisted_edwards_curve_t input_curve) {
    quadratic_field_element_t A, B, C, D, E, F, G;
    quadratic_field_multiplication(&A, input_P.Z, input_Q.Z);
    quadratic_field_square(&B, A);
    quadratic_field_multiplication(&C, input_P.X, input_Q.X);
    quadratic_field_multiplication(&D, input_P.Y, input_Q.Y);

    quadratic_field_multiplication(&E, input_curve.d, C);
    quadratic_field_multiplication(&E, E, D);

    quadratic_field_subtraction(&F, B, E);
    quadratic_field_addition(&G, B, E);

    quadratic_field_addition(&B, input_P.X, input_P.Y);
    quadratic_field_addition(&E, input_Q.X, input_Q.Y);
    quadratic_field_multiplication(&output->X, B, E);
    quadratic_field_subtraction(&output->X, output->X, C);
    quadratic_field_subtraction(&output->X, output->X, D);
    quadratic_field_multiplication(&output->X, F, output->X);
    quadratic_field_multiplication(&output->X, A, output->X);

    quadratic_field_multiplication(&B, input_curve.a, C);
    quadratic_field_subtraction(&output->Y, D, B);
    quadratic_field_multiplication(&output->Y, G, output->Y);
    quadratic_field_multiplication(&output->Y, A, output->Y);

    quadratic_field_multiplication(&output->Z, F, G);
}

void projective_point_tripling(projective_point_t *output,
                               projective_point_t input_P,
                               quadratic_field_element_t input_a) {
    quadratic_field_element_t YY, aXX, Ap, B, xB, yB, AA, F, G;
    quadratic_field_square(&YY, input_P.Y);

    quadratic_field_square(&aXX, input_P.X);
    quadratic_field_multiplication(&aXX, input_a, aXX);

    quadratic_field_addition(&Ap, YY, aXX);

    quadratic_field_square(&B, input_P.Z);
    quadratic_field_addition(&B, B, B);
    quadratic_field_subtraction(&B, B, Ap);
    quadratic_field_addition(&B, B, B);

    quadratic_field_multiplication(&xB, aXX, B);
    quadratic_field_multiplication(&yB, YY, B);

    quadratic_field_subtraction(&AA, YY, aXX);
    quadratic_field_multiplication(&AA, Ap, AA);

    quadratic_field_subtraction(&F, AA, yB);
    quadratic_field_addition(&G, AA, xB);

    quadratic_field_addition(&B, yB, AA);
    quadratic_field_multiplication(&output->X, input_P.X, B);
    quadratic_field_multiplication(&output->X, output->X, F);

    quadratic_field_subtraction(&B, xB, AA);
    quadratic_field_multiplication(&output->Y, input_P.Y, B);
    quadratic_field_multiplication(&output->Y, output->Y, G);

    quadratic_field_multiplication(&output->Z, input_P.Z, F);
    quadratic_field_multiplication(&output->Z, output->Z, G);
}

void projective_point_doubling(projective_point_t *output,
                               projective_point_t input_P,
                               quadratic_field_element_t input_a) {
    quadratic_field_element_t B, C, E, D, F, H, J, t;

    quadratic_field_addition(&B, input_P.X, input_P.Y);
    quadratic_field_square(&B, B);

    quadratic_field_square(&C, input_P.X);
    quadratic_field_square(&D, input_P.Y);
    quadratic_field_multiplication(&E, input_a, C);
    quadratic_field_addition(&F, E, D);
    quadratic_field_square(&H, input_P.Z);

    quadratic_field_addition(&t, H, H);
    quadratic_field_subtraction(&J, F, t);

    quadratic_field_subtraction(&t, B, C);
    quadratic_field_subtraction(&t, t, D);
    quadratic_field_multiplication(&output->X, t, J);

    quadratic_field_subtraction(&t, E, D);
    quadratic_field_multiplication(&output->Y, F, t);

    quadratic_field_multiplication(&output->Z, F, J);
}

void projective_point_copy(projective_point_t *output, projective_point_t input) {
    quadratic_field_copy(&output->X, input.X);
    quadratic_field_copy(&output->Y, input.Y);
    quadratic_field_copy(&output->Z, input.Z);
}

uint8_t projective_point_is_infinity(projective_point_t input) {
    return quadratic_field_is_zero(input.X) & quadratic_field_is_equal(input.Y, input.Z);
}

uint8_t projective_point_is_equal(projective_point_t input_P, projective_point_t input_Q) {
    quadratic_field_element_t s, t, u, v;
    quadratic_field_multiplication(&s, input_Q.X, input_P.Z);
    quadratic_field_multiplication(&t, input_Q.Z, input_P.X);
    quadratic_field_multiplication(&u, input_Q.Y, input_P.Z);
    quadratic_field_multiplication(&v, input_Q.Z, input_P.Y);
    return quadratic_field_is_equal(s, t) & quadratic_field_is_equal(u, v);
}

uint8_t projective_point_is_negative(projective_point_t input_P, projective_point_t input_Q) {
    quadratic_field_element_t s, t, u, v;
    quadratic_field_multiplication(&s, input_Q.X, input_P.Z);
    quadratic_field_multiplication(&t, input_Q.Z, input_P.X);
    quadratic_field_multiplication(&u, input_Q.Y, input_P.Z);
    quadratic_field_multiplication(&v, input_Q.Z, input_P.Y);
    quadratic_field_negate(&s, s);
    return quadratic_field_is_equal(s, t) & quadratic_field_is_equal(u, v);
}

uint8_t projective_point_is_equal_or_negative(projective_point_t input_P, projective_point_t input_Q) {
    quadratic_field_element_t s, s_negate, t, u, v;
    quadratic_field_multiplication(&s, input_Q.X, input_P.Z);
    quadratic_field_multiplication(&t, input_Q.Z, input_P.X);
    quadratic_field_multiplication(&u, input_Q.Y, input_P.Z);
    quadratic_field_multiplication(&v, input_Q.Z, input_P.Y);
    quadratic_field_negate(&s_negate, s);
    uint8_t positive = quadratic_field_is_equal(s, t) & quadratic_field_is_equal(u, v),
            negative = quadratic_field_is_equal(s_negate, t) & quadratic_field_is_equal(u, v);
    return positive ^ (-negative);
}

void pohlig_hellman_step_bob(projective_point_t *T,
                             uint64_t output_c[SUBGROUP_ORDER_64BITS_WORDS],
                             uint64_t output_d[SUBGROUP_ORDER_64BITS_WORDS],
                             projective_point_t P,
                             projective_point_t Q,
                             projective_point_t R,
                             const projective_point_t table[4],
                             twisted_edwards_curve_t curve,
                             uint64_t coefficient[SUBGROUP_ORDER_64BITS_WORDS]) {
    projective_point_t S;
    uint8_t decision;
    decision = projective_point_is_equal_or_negative(table[0], R);
    if (decision == 0x01) {
        // cᵢ = 1 and dᵢ = 0
        multiprecision_addition(output_c, output_c, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_copy(&S, P);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    } else if (decision == 0xFF) {
        // cᵢ = 2 and dᵢ = 0
        multiprecision_shift_to_left(coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(output_c, output_c, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_doubling(&S, P, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    }
    decision = projective_point_is_equal_or_negative(table[1], R);
    if (decision == 0x01) {
        // cᵢ = 0 and dᵢ = 1
        multiprecision_addition(output_d, output_d, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_copy(&S, Q);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    } else if (decision == 0xFF) {
        // cᵢ = 0 and dᵢ = 2
        multiprecision_shift_to_left(coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(output_d, output_d, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_doubling(&S, Q, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    }
    decision = projective_point_is_equal_or_negative(table[2], R);
    if (decision == 0x01) {
        // cᵢ = 1 and dᵢ = 1
        multiprecision_addition(output_c, output_c, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(output_d, output_d, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_addition(&S, P, Q, curve);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    } else if (decision == 0xFF) {
        // cᵢ = 2 and dᵢ = 2
        multiprecision_shift_to_left(coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(output_c, output_c, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(output_d, output_d, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_addition(&S, P, Q, curve);
        projective_point_doubling(&S, S, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    }
    decision = projective_point_is_equal_or_negative(table[3], R);
    if (decision == 0x01) {
        // cᵢ = 1 and dᵢ = 2
        multiprecision_addition(output_c, output_c, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_copy(&S, P);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);

        multiprecision_shift_to_left(coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(output_d, output_d, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_doubling(&S, Q, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    } else if (decision == 0xFF) {
        // cᵢ = 2 and dᵢ = 1
        multiprecision_addition(output_d, output_d, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_copy(&S, Q);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);

        multiprecision_shift_to_left(coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(output_c, output_c, coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        projective_point_doubling(&S, P, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    }
}

void pohlig_hellman_steps_bob(projective_point_t *T,
                              projective_point_t P,
                              projective_point_t Q,
                              projective_point_t R,
                              const projective_point_t table[4],
                              twisted_edwards_curve_t curve) {
    projective_point_t S;
    uint8_t decision;
    decision = projective_point_is_equal_or_negative(table[0], R);
    if (decision == 0x01) {
        // cᵢ = 1 and dᵢ = 0
        projective_point_copy(&S, P);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    } else if (decision == 0xFF) {
        // cᵢ = 2 and dᵢ = 0
        projective_point_doubling(&S, P, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    }
    decision = projective_point_is_equal_or_negative(table[1], R);
    if (decision == 0x01) {
        // cᵢ = 0 and dᵢ = 1
        projective_point_copy(&S, Q);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    } else if (decision == 0xFF) {
        // cᵢ = 0 and dᵢ = 2
        projective_point_doubling(&S, Q, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    }
    decision = projective_point_is_equal_or_negative(table[2], R);
    if (decision == 0x01) {
        // cᵢ = 1 and dᵢ = 1
        projective_point_addition(&S, P, Q, curve);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    } else if (decision == 0xFF) {
        // cᵢ = 2 and dᵢ = 2
        projective_point_addition(&S, P, Q, curve);
        projective_point_doubling(&S, S, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    }
    decision = projective_point_is_equal_or_negative(table[3], R);
    if (decision == 0x01) {
        // cᵢ = 1 and dᵢ = 2
        projective_point_copy(&S, P);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);

        projective_point_doubling(&S, Q, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    } else if (decision == 0xFF) {
        // cᵢ = 2 and dᵢ = 1
        projective_point_copy(&S, Q);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);

        projective_point_doubling(&S, P, curve.a);
        quadratic_field_negate(&S.X, S.X);
        projective_point_addition(T, *T, S, curve);
    }
}

void two_dimensional_pohlig_hellman_bob(uint64_t output_c[SUBGROUP_ORDER_64BITS_WORDS],
                                        uint64_t output_d[SUBGROUP_ORDER_64BITS_WORDS],
                                        projective_point_t input_K,
                                        projective_point_t input_P,
                                        projective_point_t input_Q,
                                        twisted_edwards_curve_t curve) {
    uint8_t i, row, strategy_step, index = 0,
            strategy_indexes[STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB],
            number_of_points = 0, strategy_position = 0;

    uint64_t monomial[SUBGROUP_ORDER_64BITS_WORDS] = {0}, coefficient[SUBGROUP_ORDER_64BITS_WORDS] = {0};
    projective_point_t P_multiples[EXPONENT_BOB], Q_multiples[EXPONENT_BOB], table[4], R, T,
            strategy_points[STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB];

    memset(output_c, 0, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
    memset(output_d, 0, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
    memset(monomial, 0, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);   // monomial will represent 3ⁱ
    monomial[0] = 1;
    memset(coefficient, 0, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);

    projective_point_copy(&P_multiples[0], input_P);    // P
    projective_point_copy(&Q_multiples[0], input_Q);    // Q
    projective_point_copy(&R, input_K);     // K

    for (row = 1; row < EXPONENT_BOB; row++) {
        projective_point_tripling(&P_multiples[row], P_multiples[row - 1], curve.a);
        projective_point_tripling(&Q_multiples[row], Q_multiples[row - 1], curve.a);
    }

    projective_point_copy(&table[0], P_multiples[EXPONENT_BOB - 1]);    // [3ᵇ⁻¹]P
    projective_point_copy(&table[1], Q_multiples[EXPONENT_BOB - 1]);    // [3ᵇ⁻¹]Q
    projective_point_addition(&table[2],
                              P_multiples[EXPONENT_BOB - 1],
                              Q_multiples[EXPONENT_BOB - 1],
                              curve);   // [3ᵇ⁻¹](P + Q)
    projective_point_addition(&table[3],
                              table[2],
                              Q_multiples[EXPONENT_BOB - 1],
                              curve);   // [3ᵇ⁻¹](P + [2]Q)

    for (row = 1; row < EXPONENT_BOB; row++) {
        while (index < NUMBER_OF_ISOGENIES_BOB - row) {
            projective_point_copy(&strategy_points[number_of_points], R);
            strategy_indexes[number_of_points++] = index;
            strategy_step = STRATEGY_POHLIG_HELLMAN[strategy_position++];
            for (i = 0; i < strategy_step; i++) { projective_point_tripling(&R, R, curve.a); }
            index += strategy_step;
        }

        memcpy(coefficient, monomial, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);  // 3ⁱ
        pohlig_hellman_step_bob(&T,
                                output_c,
                                output_d,
                                P_multiples[row - 1],
                                Q_multiples[row - 1],
                                R,
                                table,
                                curve,
                                coefficient);
        for (i = 0; i < number_of_points; i++) {
            pohlig_hellman_steps_bob(&strategy_points[i],
                                     P_multiples[row - 1 + strategy_indexes[i]],
                                     Q_multiples[row - 1 + strategy_indexes[i]],
                                     R,
                                     table,
                                     curve);
        }
        // multiplication by 3
        memcpy(coefficient, monomial, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_shift_to_left(coefficient, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(monomial, monomial, coefficient, SUBGROUP_ORDER_64BITS_WORDS);

        projective_point_copy(&R, strategy_points[number_of_points - 1]);
        index = strategy_indexes[number_of_points - 1];
        number_of_points -= 1;
    }
    projective_point_copy(&strategy_points[number_of_points], R);
    memcpy(coefficient, monomial, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);  // 3ᵇ⁻¹
    pohlig_hellman_step_bob(&T,
                            output_c,
                            output_d,
                            P_multiples[EXPONENT_BOB - 1],
                            Q_multiples[EXPONENT_BOB - 1],
                            strategy_points[number_of_points],
                            table,
                            curve,
                            coefficient);
}

void to_twisted_edwards_model(projective_point_t *output,
                              quadratic_field_element_t input_x,
                              quadratic_field_element_t input_y) {

    quadratic_field_set_to_one(&output->Z);
    quadratic_field_subtraction(&output->Y, input_x, output->Z);    // x - 1
    quadratic_field_multiplication(&output->Y, output->Y, input_y); // y(x - 1)
    quadratic_field_addition(&output->Z, input_x, output->Z);       // x + 1
    quadratic_field_multiplication(&output->X, output->Z, input_x); // x(x + 1)
    quadratic_field_addition(&output->Z, output->Y, input_y);       // yx
    quadratic_field_addition(&output->Z, output->Z, input_y);       // y(x + 1)
}

void to_twisted_edwards_model_mixed(projective_point_t *output,
                                    quadratic_field_element_t input_x,
                                    quadratic_field_element_t input_y,
                                    quadratic_field_element_t input_z) {

    quadratic_field_set_to_one(&output->Z);
    quadratic_field_subtraction(&output->Y, input_x, output->Z);    // x - 1
    quadratic_field_multiplication(&output->Y, output->Y, input_y); // y(x - 1)
    quadratic_field_addition(&output->Z, input_x, output->Z);       // x + 1
    quadratic_field_multiplication(&output->X, output->Z, input_x); // x(x + 1)
    quadratic_field_multiplication(&output->X, output->X, input_z);
    quadratic_field_addition(&output->Z, output->Y, input_y);       // yx
    quadratic_field_addition(&output->Z, output->Z, input_y);       // y(x + 1)
}
