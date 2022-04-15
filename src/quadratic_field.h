//
// Quadratic field arithmetic assuming p = 3 mod 4: GF(p²)
//

#ifndef SIDH_POK_QUADRATIC_FIELD_H
#define SIDH_POK_QUADRATIC_FIELD_H

#include "prime_field.h"

typedef struct {
    prime_field_element_t re;   // real part: a
    prime_field_element_t im;   // imaginary part: b
} quadratic_field_element_t;    // Elements of the form a + ib where i²=-1

void quadratic_field_set_to_one(quadratic_field_element_t *output);

void quadratic_field_set_to_zero(quadratic_field_element_t *output);

void quadratic_field_copy(quadratic_field_element_t *output, quadratic_field_element_t input);

void quadratic_field_addition(quadratic_field_element_t *output,
                              quadratic_field_element_t input_a, quadratic_field_element_t input_b);

void quadratic_field_subtraction(quadratic_field_element_t *output,
                                 quadratic_field_element_t input_a, quadratic_field_element_t input_b);

void quadratic_field_multiplication(quadratic_field_element_t *output,
                                    quadratic_field_element_t input_a, quadratic_field_element_t input_b);

void quadratic_field_square(quadratic_field_element_t *output, quadratic_field_element_t input);

void quadratic_field_inverse(quadratic_field_element_t *output, quadratic_field_element_t input);

void quadratic_field_negate(quadratic_field_element_t *output, quadratic_field_element_t input);

void quadratic_field_divide_by_2(quadratic_field_element_t *output, quadratic_field_element_t input);

void quadratic_field_to_montgomery_representation(quadratic_field_element_t *output, quadratic_field_element_t input);

void quadratic_field_from_montgomery_representation(quadratic_field_element_t *output, quadratic_field_element_t input);

uint8_t quadratic_field_is_zero(quadratic_field_element_t input);

uint8_t quadratic_field_is_equal(quadratic_field_element_t input_a, quadratic_field_element_t input_b);

void quadratic_field_conjugate(quadratic_field_element_t *output, quadratic_field_element_t input);

uint8_t quadratic_field_is_square(quadratic_field_element_t *output, quadratic_field_element_t input);

void quadratic_field_element_to_bytes(uint8_t *output, quadratic_field_element_t input);

void quadratic_field_element_from_bytes(quadratic_field_element_t *output, const uint8_t *input);

#endif //SIDH_POK_QUADRATIC_FIELD_H
