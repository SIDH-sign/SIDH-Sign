//
//  Utility functions: 64-bits built-in functions
//

#ifndef SIDH_POK_UTILITIES_H
#define SIDH_POK_UTILITIES_H

#include <stdint.h>

void multiplication_u64(uint64_t *output_low, uint64_t *output_high, uint64_t input_a, uint64_t input_b);

void conditional_move_u64(uint64_t *output, uint8_t input_decision, uint64_t input_a, uint64_t input_b);

uint64_t constant_time_is_nonzero_u64(uint64_t x);

uint64_t constant_time_is_zero_u64(uint64_t x);

uint64_t constant_time_is_lessthan_u64(uint64_t x, uint64_t y);

int8_t constant_time_compare(const uint8_t *input_a, const uint8_t *input_b, uint64_t input_length);

void
constant_time_conditional_mov(uint8_t *output, const uint8_t *input, uint64_t input_length, uint8_t input_selector);

void multiprecision_shift_to_right(uint64_t *input_a_output_shifted_a, uint64_t input_words_length);

void multiprecision_shift_to_left(uint64_t *input_a_output_shifted_a, uint64_t input_words_length);

#define addition_with_carry_u64(output, output_carry, input_carry, input_a, input_b)    \
    { uint64_t tmp = (input_a) + (uint64_t)(input_carry);                               \
    (output) = (input_b) + tmp;                                                         \
    (output_carry) = (constant_time_is_lessthan_u64(tmp, (uint64_t)(input_carry)) | constant_time_is_lessthan_u64((output), tmp)); }

#define subtraction_with_borrow_u64(output, output_borrow, input_borrow, input_a, input_b)                          \
    { uint64_t tmp = (input_a) - (input_b);                                                                         \
    uint64_t borrowReg = (constant_time_is_lessthan_u64((input_a), (input_b)) | ((input_borrow) & constant_time_is_zero_u64(tmp)));   \
    (output) = tmp - (uint64_t)(input_borrow);                                                                      \
    (output_borrow) = borrowReg; }

#define constant_time_shift_to_right_u64(shiftOut, highIn, lowIn, shift, DigitSize)  \
    (shiftOut) = ((lowIn) >> (shift)) ^ ((highIn) << ((DigitSize) - (shift)));

#define constant_time_shift_to_left_u64(shiftOut, highIn, lowIn, shift, DigitSize)  \
    (shiftOut) = ((highIn) << (shift)) ^ ((lowIn) >> ((DigitSize) - (shift)));

void multiprecision_addition(uint64_t *output, const uint64_t *input_a, const uint64_t *input_b, uint64_t input_length);

void
multiprecision_subtraction(uint64_t *output, const uint64_t *input_a, const uint64_t *input_b, uint64_t input_length);

uint8_t multiprecision_is_smaller(const uint64_t *input1, const uint64_t *input2, uint64_t input_length);

#endif //SIDH_POK_UTILITIES_H
