//
// Supersingular elliptic curve (Kummer line arithmetic): y² = x³ + Ax² + x
//

#ifndef SIDH_POK_ELLIPTIC_CURVE_H
#define SIDH_POK_ELLIPTIC_CURVE_H

#include "quadratic_field.h"

typedef uint64_t scalar_t[SUBGROUP_ORDER_64BITS_WORDS];

typedef struct {
    quadratic_field_element_t X;
    quadratic_field_element_t Z;
} x_only_point_t; // projective x-coordinate point (X : Z) determining Pₓ = X/Z

typedef struct {
    quadratic_field_element_t A_plus_2C;
    quadratic_field_element_t C_times_4;
} projective_curve_alice_t; // projective curve A-coefficient (A' + 2C : 4C) determining A = A'/C

typedef struct {
    quadratic_field_element_t A_plus_2C;
    quadratic_field_element_t A_minus_2C;
} projective_curve_bob_t; // projective curve A-coefficient (A' + 2C : A - 2C) determining A = A'/C

void x_only_point_copy(x_only_point_t *output, x_only_point_t input);

void x_only_point_doubling(x_only_point_t *output,
                           x_only_point_t input_P,
                           projective_curve_alice_t input_curve);

void x_only_multiple_point_doublings(x_only_point_t *output_2_raised_to_e_P,
                                     x_only_point_t input_P,
                                     uint64_t input_e,
                                     projective_curve_alice_t input_curve);

void x_only_point_tripling(x_only_point_t *output,
                           x_only_point_t input_P,
                           projective_curve_bob_t input_curve);

void x_only_multiple_point_triplings(x_only_point_t *output_3_raised_to_e_P,
                                     x_only_point_t input_P,
                                     uint64_t input_e,
                                     projective_curve_bob_t input_curve);

void x_only_point_simultaneous_double_and_addition(x_only_point_t *input_P_output_P2,
                                                   x_only_point_t *input_Q_output_P_plus_Q,
                                                   x_only_point_t input_P_minus_Q,
                                                   quadratic_field_element_t input_quarter_of_A_plus_2);

void x_only_point_swap(x_only_point_t *input_P, x_only_point_t *input_Q, uint64_t input_option);

void x_only_three_point_ladder(x_only_point_t *output_P_plus_kQ,
                               quadratic_field_element_t input_P,
                               quadratic_field_element_t input_Q,
                               quadratic_field_element_t input_P_minus_Q,
                               const scalar_t input_k,
                               int input_bits_of_k,
                               quadratic_field_element_t input_A);

void to_alice_curve_representation(projective_curve_alice_t *output, projective_curve_bob_t input);

void to_projective_curve_alice(projective_curve_alice_t *output, quadratic_field_element_t input);

void from_projective_curve_alice(quadratic_field_element_t *output, projective_curve_alice_t input);

void to_projective_curve_bob(projective_curve_bob_t *output, quadratic_field_element_t input);

void from_projective_curve_bob(quadratic_field_element_t *output, projective_curve_bob_t input);

void x_only_point_set_to_infinity(x_only_point_t *input);

uint8_t x_only_point_is_infinity(x_only_point_t P);

uint8_t x_only_point_is_equal(x_only_point_t input_P, x_only_point_t input_Q);

void x_only_point_addition(x_only_point_t *output,
                           x_only_point_t input_P, x_only_point_t input_Q,
                           x_only_point_t input_P_minus_Q);

void j_invariant(quadratic_field_element_t *output, projective_curve_alice_t input);

// Have in mind P is reduced to have torsion 3ᵇ and thus P <-[2ᵃ]P
uint8_t x_only_is_full_order_bob(x_only_point_t *P3, x_only_point_t *P, quadratic_field_element_t A);

// Elligator assumes u is a Quadratic Non Residue (QNR), and A ≠ 0
uint8_t x_only_elligator2(x_only_point_t *P,
                          quadratic_field_element_t u,
                          quadratic_field_element_t r_squared,
                          quadratic_field_element_t A);

void x_only_get_P_minus_Q(x_only_point_t *P_minus_Q,
                          quadratic_field_element_t xP,
                          quadratic_field_element_t xQ,
                          quadratic_field_element_t z,
                          quadratic_field_element_t u_prime,
                          quadratic_field_element_t r,
                          quadratic_field_element_t A);

void x_only_canonical_basis_bob(x_only_point_t *P,
                                x_only_point_t *Q,
                                x_only_point_t *P_minus_Q,
                                quadratic_field_element_t u_prime,
                                quadratic_field_element_t A);


void y_coordinate_recovery_projective(quadratic_field_element_t *yQ,
                                      quadratic_field_element_t *zQ,
                                      quadratic_field_element_t xP,
                                      quadratic_field_element_t yP,
                                      quadratic_field_element_t xQ,
                                      quadratic_field_element_t x_P_minus_Q,
                                      quadratic_field_element_t A);

// Next two ladder procedures are non-constant-time
void x_only_ladder_bob(x_only_point_t *output_kP,
                       x_only_point_t input_P,
                       const scalar_t input_k,
                       int input_bits_of_k,
                       quadratic_field_element_t input_A);

void two_dimensional_scalar_multiplication_bob(x_only_point_t *output_cP_plus_dQ,
                                               quadratic_field_element_t input_P,
                                               quadratic_field_element_t input_Q,
                                               quadratic_field_element_t input_P_minus_Q,
                                               const scalar_t input_c,
                                               const scalar_t input_d,
                                               quadratic_field_element_t input_A);

// No inverse computation
uint8_t x_only_point_is_equal_mixed(x_only_point_t input_P, quadratic_field_element_t input_xP);

// No inverse computation
uint8_t projective_curve_is_equal_mixed_alice(projective_curve_alice_t input_curve, quadratic_field_element_t input_A);

// No inverse computation
uint8_t projective_curve_is_equal_mixed_bob(projective_curve_bob_t input_curve, quadratic_field_element_t input_A);

#endif //SIDH_POK_ELLIPTIC_CURVE_H
