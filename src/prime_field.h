//
// Prime field arithmetic GF(p)
//

#ifndef SIDH_POK_PRIME_FIELD_H
#define SIDH_POK_PRIME_FIELD_H

#include <stdint.h>
#include "fips202.h"
#include "parameters.h"

typedef uint64_t prime_field_element_t[FIELD_64BITS_WORDS];

void prime_field_multiplication(prime_field_element_t output,
                                const prime_field_element_t input_a, const prime_field_element_t input_b);

void prime_field_square(prime_field_element_t output, const prime_field_element_t input);

void prime_field_addition(prime_field_element_t output,
                          const prime_field_element_t input_a, const prime_field_element_t input_b);

void prime_field_subtraction(prime_field_element_t output,
                             const prime_field_element_t input_a, const prime_field_element_t input_b);

void prime_field_negate(prime_field_element_t output, const prime_field_element_t input);

/*
 * Funcitons implemente in prime_field.c
 */

void prime_field_set_to_one(prime_field_element_t output);

void prime_field_set_to_zero(prime_field_element_t output);

void prime_field_sample(prime_field_element_t output, keccak_state *state);

void prime_field_copy(prime_field_element_t output, const prime_field_element_t input);

void prime_field_inverse(prime_field_element_t output, const prime_field_element_t input);

void prime_field_divide_by_2(prime_field_element_t output, const prime_field_element_t input);

void prime_field_to_montgomery_representation(prime_field_element_t output, const prime_field_element_t input);

void prime_field_from_montgomery_representation(prime_field_element_t output, const prime_field_element_t input);

uint8_t prime_field_is_zero(const prime_field_element_t input);

uint8_t prime_field_is_equal(const prime_field_element_t input_a, const prime_field_element_t input_b);

uint8_t prime_field_is_smaller(const prime_field_element_t input1, const prime_field_element_t input2);

uint8_t prime_field_is_square(const prime_field_element_t input);

#endif //SIDH_POK_PRIME_FIELD_H
