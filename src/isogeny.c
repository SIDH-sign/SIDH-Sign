//
// Isogenies connecting Montgomery curves: 2ᵃ-isogenies and 3ᵇ-isogenies
//

#include "isogeny.h"

void initialize_basis(quadratic_field_element_t *XP,
                      quadratic_field_element_t *XQ,
                      quadratic_field_element_t *XR,
                      const uint64_t *basis) {
    prime_field_copy(XP->re, basis);
    prime_field_copy(XP->im, &basis[FIELD_64BITS_WORDS]);
    prime_field_copy(XQ->re, &basis[2 * FIELD_64BITS_WORDS]);
    prime_field_copy(XQ->im, &basis[3 * FIELD_64BITS_WORDS]);
    prime_field_copy(XR->re, &basis[4 * FIELD_64BITS_WORDS]);
    prime_field_copy(XR->im, &basis[5 * FIELD_64BITS_WORDS]);
}

#if (EXPONENT_ALICE % 2 == 1)

void x_only_2_isogeny(projective_curve_alice_t *output_curve, x_only_point_t input_kernel) {
    quadratic_field_square(&output_curve->A_plus_2C, input_kernel.X);
    quadratic_field_square(&output_curve->C_times_4, input_kernel.Z);
    quadratic_field_subtraction(&output_curve->A_plus_2C,
                                output_curve->C_times_4,
                                output_curve->A_plus_2C);
}


void x_only_2_isogeny_evaluation(x_only_point_t *output, x_only_point_t input, x_only_point_t input_kernel) {
    quadratic_field_element_t t0, t1, t2, t3;

    quadratic_field_addition(&t0, input_kernel.X, input_kernel.Z);
    quadratic_field_subtraction(&t1, input_kernel.X, input_kernel.Z);
    quadratic_field_addition(&t2, input.X, input.Z);
    quadratic_field_subtraction(&t3, input.X, input.Z);
    quadratic_field_multiplication(&t0, t3, t0);
    quadratic_field_multiplication(&t1, t2, t1);
    quadratic_field_addition(&t2, t0, t1);
    quadratic_field_subtraction(&t3, t0, t1);
    quadratic_field_multiplication(&output->X, t2, input.X);
    quadratic_field_multiplication(&output->Z, t3, input.Z);
}

#endif

void x_only_4_isogeny(projective_curve_alice_t *output_curve,
                      quadratic_field_element_t output_isogeny_coefficients[3],
                      x_only_point_t input_kernel) {
    quadratic_field_subtraction(&output_isogeny_coefficients[1], input_kernel.X, input_kernel.Z);
    quadratic_field_addition(&output_isogeny_coefficients[2], input_kernel.X, input_kernel.Z);
    quadratic_field_square(&output_isogeny_coefficients[0], input_kernel.Z);
    quadratic_field_addition(&output_isogeny_coefficients[0],
                             output_isogeny_coefficients[0],
                             output_isogeny_coefficients[0]);
    quadratic_field_square(&output_curve->C_times_4, output_isogeny_coefficients[0]);
    quadratic_field_addition(&output_isogeny_coefficients[0],
                             output_isogeny_coefficients[0],
                             output_isogeny_coefficients[0]);
    quadratic_field_square(&output_curve->A_plus_2C, input_kernel.X);
    quadratic_field_addition(&output_curve->A_plus_2C,
                             output_curve->A_plus_2C,
                             output_curve->A_plus_2C);
    quadratic_field_square(&output_curve->A_plus_2C, output_curve->A_plus_2C);
}


void x_only_4_isogeny_evaluation(x_only_point_t *output,
                                 x_only_point_t input,
                                 const quadratic_field_element_t input_isogeny_coefficients[3]) {
    quadratic_field_element_t t0, t1;

    quadratic_field_addition(&t0, input.X, input.Z);
    quadratic_field_subtraction(&t1, input.X, input.Z);
    quadratic_field_multiplication(&output->X, t0, input_isogeny_coefficients[1]);
    quadratic_field_multiplication(&output->Z, t1, input_isogeny_coefficients[2]);
    quadratic_field_multiplication(&t0, t0, t1);
    quadratic_field_multiplication(&t0, input_isogeny_coefficients[0], t0);
    quadratic_field_addition(&t1, output->X, output->Z);
    quadratic_field_subtraction(&output->Z, output->X, output->Z);
    quadratic_field_square(&t1, t1);
    quadratic_field_square(&output->Z, output->Z);
    quadratic_field_addition(&output->X, t1, t0);
    quadratic_field_subtraction(&t0, output->Z, t0);
    quadratic_field_multiplication(&output->X, output->X, t1);
    quadratic_field_multiplication(&output->Z, output->Z, t0);
}

void x_only_isogeny_from_kernel_alice(x_only_point_t *input_output_P,
                                      x_only_point_t *input_output_Q,
                                      x_only_point_t *input_output_P_minus_Q,
                                      projective_curve_alice_t *curve,
                                      x_only_point_t input_kernel) {
    quadratic_field_element_t isogeny_coefficients[3];
    x_only_point_t R, strategy_points[STRATEGY_MAXIMUM_INTERNAL_POINTS_ALICE];
    uint8_t i, row, strategy_step, index = 0, strategy_indexes[STRATEGY_MAXIMUM_INTERNAL_POINTS_ALICE],
            number_of_points = 0, strategy_position = 0;

    x_only_point_copy(&R, input_kernel);
    index = 0;
#if (EXPONENT_ALICE % 2 == 1)
    // A single 2-isogeny
    x_only_point_t S;

    while (index < NUMBER_OF_ISOGENIES_ALICE - 1) {
        x_only_point_copy(&strategy_points[number_of_points], R);
        strategy_indexes[number_of_points++] = index;
        strategy_step = STRATEGY_ALICE[strategy_position++];
        x_only_multiple_point_doublings(&R,
                                        R,
                                        (int) (2 * strategy_step),
                                        *curve);
        index += strategy_step;
    }
    x_only_multiple_point_doublings(&S,
                                    R,
                                    2,
                                    *curve);
    x_only_2_isogeny(curve, S);
    for (i = 0; i < number_of_points; i++) {
        x_only_2_isogeny_evaluation(&strategy_points[i], *&strategy_points[i], S);
    }
    x_only_2_isogeny_evaluation(input_output_P, *input_output_P, S);
    x_only_2_isogeny_evaluation(input_output_Q, *input_output_Q, S);
    x_only_2_isogeny_evaluation(input_output_P_minus_Q, *input_output_P_minus_Q, S);
    x_only_2_isogeny_evaluation(&R, R, S);
#endif

    // Composition of NUMBER_OF_ISOGENIES_ALICE consecutive 4-isogenies
    for (row = 1; row < NUMBER_OF_ISOGENIES_ALICE; row++) {
        while (index < NUMBER_OF_ISOGENIES_ALICE - row) {
            x_only_point_copy(&strategy_points[number_of_points], R);
            strategy_indexes[number_of_points++] = index;
            strategy_step = STRATEGY_ALICE[strategy_position++];
            x_only_multiple_point_doublings(&R,
                                            R,
                                            (int) (2 * strategy_step),
                                            *curve);
            index += strategy_step;
        }
        x_only_4_isogeny(curve, isogeny_coefficients, R);
        for (i = 0; i < number_of_points; i++) {
            x_only_4_isogeny_evaluation(&strategy_points[i], strategy_points[i], isogeny_coefficients);
        }
        x_only_4_isogeny_evaluation(input_output_P, *input_output_P, isogeny_coefficients);
        x_only_4_isogeny_evaluation(input_output_Q, *input_output_Q, isogeny_coefficients);
        x_only_4_isogeny_evaluation(input_output_P_minus_Q, *input_output_P_minus_Q, isogeny_coefficients);

        x_only_point_copy(&R, strategy_points[number_of_points - 1]);
        index = strategy_indexes[number_of_points - 1];
        number_of_points -= 1;
    }

    x_only_4_isogeny(curve, isogeny_coefficients, R);
    x_only_4_isogeny_evaluation(input_output_P, *input_output_P, isogeny_coefficients);
    x_only_4_isogeny_evaluation(input_output_Q, *input_output_Q, isogeny_coefficients);
    x_only_4_isogeny_evaluation(input_output_P_minus_Q, *input_output_P_minus_Q, isogeny_coefficients);
}

void x_only_isogeny_from_kernel_curve_alice(projective_curve_alice_t *output_curve,
                                            x_only_point_t input_kernel,
                                            projective_curve_alice_t input_curve) {
    quadratic_field_element_t isogeny_coefficients[3];
    x_only_point_t R, strategy_points[STRATEGY_MAXIMUM_INTERNAL_POINTS_ALICE];
    uint8_t i, row, strategy_step, index, strategy_indexes[STRATEGY_MAXIMUM_INTERNAL_POINTS_ALICE],
            number_of_points = 0, strategy_position = 0;

    quadratic_field_copy(&output_curve->A_plus_2C, input_curve.A_plus_2C);
    quadratic_field_copy(&output_curve->C_times_4, input_curve.C_times_4);
    x_only_point_copy(&R, input_kernel);
    index = 0;
#if (EXPONENT_ALICE % 2 == 1)
    // A single 2-isogeny
    x_only_point_t S;

    while (index < NUMBER_OF_ISOGENIES_ALICE - 1) {
        x_only_point_copy(&strategy_points[number_of_points], R);
        strategy_indexes[number_of_points++] = index;
        strategy_step = STRATEGY_ALICE[strategy_position++];
        x_only_multiple_point_doublings(&R,
                                        R,
                                        (int) (2 * strategy_step),
                                        *output_curve);
        index += strategy_step;
    }
    x_only_multiple_point_doublings(&S,
                                    R,
                                    2,
                                    *output_curve);
    x_only_2_isogeny(output_curve, S);
    for (i = 0; i < number_of_points; i++) {
        x_only_2_isogeny_evaluation(&strategy_points[i], *&strategy_points[i], S);
    }
    x_only_2_isogeny_evaluation(&R, R, S);
#endif

    // Composition of NUMBER_OF_ISOGENIES_ALICE consecutive 4-isogenies
    for (row = 1; row < NUMBER_OF_ISOGENIES_ALICE; row++) {
        while (index < NUMBER_OF_ISOGENIES_ALICE - row) {
            x_only_point_copy(&strategy_points[number_of_points], R);
            strategy_indexes[number_of_points++] = index;
            strategy_step = STRATEGY_ALICE[strategy_position++];
            x_only_multiple_point_doublings(&R,
                                            R,
                                            (int) (2 * strategy_step),
                                            *output_curve);
            index += strategy_step;
        }
        x_only_4_isogeny(output_curve, isogeny_coefficients, R);
        for (i = 0; i < number_of_points; i++) {
            x_only_4_isogeny_evaluation(&strategy_points[i], strategy_points[i], isogeny_coefficients);
        }

        x_only_point_copy(&R, strategy_points[number_of_points - 1]);
        index = strategy_indexes[number_of_points - 1];
        number_of_points -= 1;
    }

    x_only_4_isogeny(output_curve, isogeny_coefficients, R);
}

void x_only_3_isogeny(projective_curve_bob_t *output_curve,
                      quadratic_field_element_t output_isogeny_coefficients[2],
                      x_only_point_t input_kernel) {
    quadratic_field_element_t t0, t1, t2, t3, t4;

    quadratic_field_subtraction(&output_isogeny_coefficients[0], input_kernel.X, input_kernel.Z);
    quadratic_field_square(&t0, output_isogeny_coefficients[0]);
    quadratic_field_addition(&output_isogeny_coefficients[1], input_kernel.X, input_kernel.Z);
    quadratic_field_square(&t1, output_isogeny_coefficients[1]);
    quadratic_field_addition(&t3, input_kernel.X, input_kernel.X);
    quadratic_field_square(&t3, t3);
    quadratic_field_subtraction(&t2, t3, t0);
    quadratic_field_subtraction(&t3, t3, t1);
    quadratic_field_addition(&t4, t0, t3);
    quadratic_field_addition(&t4, t4, t4);
    quadratic_field_addition(&t4, t1, t4);
    quadratic_field_multiplication(&output_curve->A_minus_2C, t2, t4);
    quadratic_field_addition(&t4, t1, t2);
    quadratic_field_addition(&t4, t4, t4);
    quadratic_field_addition(&t4, t0, t4);
    quadratic_field_multiplication(&output_curve->A_plus_2C, t3, t4);
}


void x_only_3_isogeny_evaluation(x_only_point_t *output,
                                 x_only_point_t input,
                                 const quadratic_field_element_t input_isogeny_coefficients[2]) {
    quadratic_field_element_t t0, t1, t2;

    quadratic_field_addition(&t0, input.X, input.Z);
    quadratic_field_subtraction(&t1, input.X, input.Z);
    quadratic_field_multiplication(&t0, input_isogeny_coefficients[0], t0);
    quadratic_field_multiplication(&t1, input_isogeny_coefficients[1], t1);
    quadratic_field_addition(&t2, t0, t1);
    quadratic_field_subtraction(&t0, t1, t0);
    quadratic_field_square(&t2, t2);
    quadratic_field_square(&t0, t0);
    quadratic_field_multiplication(&output->X, input.X, t2);
    quadratic_field_multiplication(&output->Z, input.Z, t0);
}

void x_only_isogeny_from_kernel_bob(x_only_point_t *input_output_P,
                                    x_only_point_t *input_output_Q,
                                    x_only_point_t *input_output_P_minus_Q,
                                    projective_curve_bob_t *input_output_curve,
                                    x_only_point_t input_kernel) {
    quadratic_field_element_t isogeny_coefficients[2];
    x_only_point_t R, strategy_points[STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB];
    uint8_t i, row, strategy_step, index = 0, strategy_indexes[STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB],
            number_of_points = 0, strategy_position = 0;

    x_only_point_copy(&R, input_kernel);

    // Composition of NUMBER_OF_ISOGENIES_ALICE consecutive 3-isogenies
    index = 0;
    for (row = 1; row < NUMBER_OF_ISOGENIES_BOB; row++) {
        while (index < NUMBER_OF_ISOGENIES_BOB - row) {
            x_only_point_copy(&strategy_points[number_of_points], R);
            strategy_indexes[number_of_points++] = index;
            strategy_step = STRATEGY_BOB[strategy_position++];
            x_only_multiple_point_triplings(&R,
                                            R,
                                            (int) strategy_step,
                                            *input_output_curve);
            index += strategy_step;
        }
        x_only_3_isogeny(input_output_curve, isogeny_coefficients, R);
        for (i = 0; i < number_of_points; i++) {
            x_only_3_isogeny_evaluation(&strategy_points[i], strategy_points[i], isogeny_coefficients);
        }
        x_only_3_isogeny_evaluation(input_output_P, *input_output_P, isogeny_coefficients);
        x_only_3_isogeny_evaluation(input_output_Q, *input_output_Q, isogeny_coefficients);
        x_only_3_isogeny_evaluation(input_output_P_minus_Q, *input_output_P_minus_Q, isogeny_coefficients);

        x_only_point_copy(&R, strategy_points[number_of_points - 1]);
        index = strategy_indexes[number_of_points - 1];
        number_of_points -= 1;
    }

    x_only_3_isogeny(input_output_curve, isogeny_coefficients, R);
    x_only_3_isogeny_evaluation(input_output_P, *input_output_P, isogeny_coefficients);
    x_only_3_isogeny_evaluation(input_output_Q, *input_output_Q, isogeny_coefficients);
    x_only_3_isogeny_evaluation(input_output_P_minus_Q, *input_output_P_minus_Q, isogeny_coefficients);
}

void x_only_isogeny_from_kernel_curve_bob(projective_curve_bob_t *output_curve,
                                          x_only_point_t input_kernel,
                                          projective_curve_bob_t input_curve) {
    quadratic_field_element_t isogeny_coefficients[2];
    x_only_point_t R, strategy_points[STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB];
    uint8_t i, row, strategy_step, index = 0, strategy_indexes[STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB],
            number_of_points = 0, strategy_position = 0;

    quadratic_field_copy(&output_curve->A_plus_2C, input_curve.A_plus_2C);
    quadratic_field_copy(&output_curve->A_minus_2C, input_curve.A_minus_2C);
    x_only_point_copy(&R, input_kernel);

    // Composition of NUMBER_OF_ISOGENIES_ALICE consecutive 3-isogenies
    index = 0;
    for (row = 1; row < NUMBER_OF_ISOGENIES_BOB; row++) {
        while (index < NUMBER_OF_ISOGENIES_BOB - row) {
            x_only_point_copy(&strategy_points[number_of_points], R);
            strategy_indexes[number_of_points++] = index;
            strategy_step = STRATEGY_BOB[strategy_position++];
            x_only_multiple_point_triplings(&R,
                                            R,
                                            (int) strategy_step,
                                            *output_curve);
            index += strategy_step;
        }
        x_only_3_isogeny(output_curve, isogeny_coefficients, R);
        for (i = 0; i < number_of_points; i++) {
            x_only_3_isogeny_evaluation(&strategy_points[i], strategy_points[i], isogeny_coefficients);
        }

        x_only_point_copy(&R, strategy_points[number_of_points - 1]);
        index = strategy_indexes[number_of_points - 1];
        number_of_points -= 1;
    }

    x_only_3_isogeny(output_curve, isogeny_coefficients, R);
}

void x_only_isogeny_from_kernel_point_and_curve_bob(x_only_point_t *input_output_P,
                                                    x_only_point_t *input_output_R,
                                                    projective_curve_bob_t *input_output_curve,
                                                    x_only_point_t input_kernel) {
    quadratic_field_element_t isogeny_coefficients[2];
    x_only_point_t R, strategy_points[STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB];
    uint8_t i, row, strategy_step, index, strategy_indexes[STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB],
            number_of_points = 0, strategy_position = 0;

    x_only_point_copy(&R, input_kernel);

    // Composition of NUMBER_OF_ISOGENIES_ALICE consecutive 3-isogenies
    index = 0;
    for (row = 1; row < NUMBER_OF_ISOGENIES_BOB; row++) {
        while (index < NUMBER_OF_ISOGENIES_BOB - row) {
            x_only_point_copy(&strategy_points[number_of_points], R);
            strategy_indexes[number_of_points++] = index;
            strategy_step = STRATEGY_BOB[strategy_position++];
            x_only_multiple_point_triplings(&R,
                                            R,
                                            (int) strategy_step,
                                            *input_output_curve);
            index += strategy_step;
        }
        x_only_3_isogeny(input_output_curve, isogeny_coefficients, R);
        for (i = 0; i < number_of_points; i++) {
            x_only_3_isogeny_evaluation(&strategy_points[i], strategy_points[i], isogeny_coefficients);
        }

        x_only_3_isogeny_evaluation(input_output_P, *input_output_P, isogeny_coefficients);
        x_only_3_isogeny_evaluation(input_output_R, *input_output_R, isogeny_coefficients);

        x_only_point_copy(&R, strategy_points[number_of_points - 1]);
        index = strategy_indexes[number_of_points - 1];
        number_of_points -= 1;
    }

    x_only_3_isogeny(input_output_curve, isogeny_coefficients, R);
    x_only_3_isogeny_evaluation(input_output_P, *input_output_P, isogeny_coefficients);
    x_only_3_isogeny_evaluation(input_output_R, *input_output_R, isogeny_coefficients);
}
