//
//  Twisted Edwards curve: ax² + y² = 1 + dx²y²
//

#ifndef SIDH_POK_TWISTED_EDWARDS_H
#define SIDH_POK_TWISTED_EDWARDS_H

#include "quadratic_field.h"

typedef struct {
    quadratic_field_element_t X;
    quadratic_field_element_t Y;
    quadratic_field_element_t Z;
} projective_point_t; // projective point (X : Y : Z) determining x = X/Z and P y = Y/Z a point on a curve

typedef struct {
    quadratic_field_element_t a;
    quadratic_field_element_t d;
} twisted_edwards_curve_t;

void projective_point_addition(projective_point_t *output,
                               projective_point_t input_P,
                               projective_point_t input_Q,
                               twisted_edwards_curve_t input_curve);

void projective_point_tripling(projective_point_t *output,
                               projective_point_t input_P,
                               quadratic_field_element_t input_a);

void projective_point_doubling(projective_point_t *output,
                               projective_point_t input_P,
                               quadratic_field_element_t input_a);

// We assume P and Q generates all E[3ᵇ]
void two_dimensional_pohlig_hellman_bob(uint64_t output_c[SUBGROUP_ORDER_64BITS_WORDS],
                                        uint64_t output_d[SUBGROUP_ORDER_64BITS_WORDS],
                                        projective_point_t input_K,
                                        projective_point_t input_P,
                                        projective_point_t input_Q,
                                        twisted_edwards_curve_t curve);

void projective_point_copy(projective_point_t *output, projective_point_t input);

uint8_t projective_point_is_infinity(projective_point_t input);

uint8_t projective_point_is_equal(projective_point_t input_P, projective_point_t input_Q);

void to_twisted_edwards_model(projective_point_t *output,
                              quadratic_field_element_t input_x,
                              quadratic_field_element_t input_y);

// It assumes affine point with y-coordinate as y/z, and x-coordinate as x.
void to_twisted_edwards_model_mixed(projective_point_t *output,
                                    quadratic_field_element_t input_x,
                                    quadratic_field_element_t input_y,
                                    quadratic_field_element_t input_z);

#endif //SIDH_POK_TWISTED_EDWARDS_H
