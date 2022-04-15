//
// Isogenies connecting Montgomery curves: 2ᵃ-isogenies and 3ᵇ-isogenies
//

#ifndef SIDH_POK_ISOGENY_H
#define SIDH_POK_ISOGENY_H

#include "elliptic_curve.h"

// +++++++ 2ᵃ-isogenies

void initialize_basis(quadratic_field_element_t *XP,
                      quadratic_field_element_t *XQ,
                      quadratic_field_element_t *XR,
                      const uint64_t *basis);

#if (EXPONENT_ALICE % 2 == 1)

void x_only_2_isogeny(projective_curve_alice_t *output_curve, x_only_point_t input_kernel);

void x_only_2_isogeny_evaluation(x_only_point_t *output, x_only_point_t input, x_only_point_t input_kernel);

#endif

void x_only_4_isogeny(projective_curve_alice_t *output_curve,
                      quadratic_field_element_t output_isogeny_coefficients[3],
                      x_only_point_t input_kernel);

void x_only_4_isogeny_evaluation(x_only_point_t *output,
                                 x_only_point_t input,
                                 const quadratic_field_element_t input_isogeny_coefficients[3]);

void x_only_isogeny_from_kernel_alice(x_only_point_t *input_output_P,
                                      x_only_point_t *input_output_Q,
                                      x_only_point_t *input_output_P_minus_Q,
                                      projective_curve_alice_t *curve,
                                      x_only_point_t input_kernel);

void x_only_isogeny_from_kernel_curve_alice(projective_curve_alice_t *output_curve,
                                            x_only_point_t input_kernel,
                                            projective_curve_alice_t input_curve);

void x_only_isogeny_from_kernel_points_alice(x_only_point_t *input_output_P,
                                             x_only_point_t *input_output_Q,
                                             x_only_point_t *input_output_P_minus_Q,
                                             x_only_point_t input_kernel,
                                             projective_curve_alice_t input_curve);

// +++++++ 3ᵇ-isogenies
void x_only_3_isogeny(projective_curve_bob_t *output_curve,
                      quadratic_field_element_t output_isogeny_coefficients[2],
                      x_only_point_t input_kernel);

void x_only_3_isogeny_evaluation(x_only_point_t *output,
                                 x_only_point_t input,
                                 const quadratic_field_element_t input_isogeny_coefficients[2]);

void x_only_isogeny_from_kernel_bob(x_only_point_t *input_output_P,
                                    x_only_point_t *input_output_Q,
                                    x_only_point_t *input_output_P_minus_Q,
                                    projective_curve_bob_t *input_output_curve,
                                    x_only_point_t input_kernel);

void x_only_isogeny_from_kernel_curve_bob(projective_curve_bob_t *output_curve,
                                          x_only_point_t input_kernel,
                                          projective_curve_bob_t input_curve);

void x_only_isogeny_from_kernel_point_and_curve_bob(x_only_point_t *input_output_P,
                                                    x_only_point_t *input_output_R,
                                                    projective_curve_bob_t *input_output_curve,
                                                    x_only_point_t input_kernel);

#endif //SIDH_POK_ISOGENY_H
