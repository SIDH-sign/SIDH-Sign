//
// Quadratic field arithmetic assuming p = 3 mod 4: GF(p²)
//

#include <string.h>
#include "quadratic_field.h"
#include "utilities.h"

#if defined(QUADRATIC_FIELD_ASM) && (FIELD_BITS != 751)

extern void fp2sqr_re_asm(prime_field_element_t input, prime_field_element_t output);

extern void fp2sqr_im_asm(prime_field_element_t input, prime_field_element_t output);

#endif

void quadratic_field_set_to_one(quadratic_field_element_t *output) {
    prime_field_set_to_one(output->re);
    prime_field_set_to_zero(output->im);
}

void quadratic_field_set_to_zero(quadratic_field_element_t *output) {
    prime_field_set_to_zero(output->re);
    prime_field_set_to_zero(output->im);
}

void quadratic_field_copy(quadratic_field_element_t *output, quadratic_field_element_t input) {
    prime_field_copy(output->re, input.re);
    prime_field_copy(output->im, input.im);
}

void quadratic_field_addition(quadratic_field_element_t *output,
                              quadratic_field_element_t input_a, quadratic_field_element_t input_b) {
    prime_field_addition(output->re, input_a.re, input_b.re);
    prime_field_addition(output->im, input_a.im, input_b.im);
}

void quadratic_field_subtraction(quadratic_field_element_t *output,
                                 quadratic_field_element_t input_a, quadratic_field_element_t input_b) {
    prime_field_subtraction(output->re, input_a.re, input_b.re);
    prime_field_subtraction(output->im, input_a.im, input_b.im);
}

void quadratic_field_multiplication(quadratic_field_element_t *output,
                                    quadratic_field_element_t input_a, quadratic_field_element_t input_b) {
    prime_field_element_t z0, z1, z2, z3, tmp;
    prime_field_addition(z0, input_a.re, input_a.im);
    prime_field_addition(z1, input_b.re, input_b.im);
    prime_field_multiplication(tmp, z0, z1);
    prime_field_multiplication(z2, input_a.re, input_b.re);
    prime_field_multiplication(z3, input_a.im, input_b.im);
    prime_field_subtraction(output->re, z2, z3);
    prime_field_subtraction(output->im, tmp, z2);
    prime_field_subtraction(output->im, output->im, z3);
}

void quadratic_field_square(quadratic_field_element_t *output, quadratic_field_element_t input) {
#if defined(QUADRATIC_FIELD_ASM) && (FIELD_BITS != 751)
    //TODO: Works fine with linux target, but perhaps it would be better to add two input
    //          parameters for fp2sqr_re_asm and avoid possible issues with memory alignment.
    //TODO: Add temp buffer and cp
    fp2sqr_re_asm(input.re, output->re);
    fp2sqr_im_asm(input.re, output->im);
#else
    prime_field_element_t z0, z1, z2;
    prime_field_addition(z0, input.re, input.re);
    prime_field_addition(z1, input.re, input.im);
    prime_field_subtraction(z2, input.re, input.im);
    prime_field_multiplication(output->re, z1, z2);
    prime_field_multiplication(output->im, z0, input.im);
#endif
}

void quadratic_field_inverse(quadratic_field_element_t *output, quadratic_field_element_t input) {
    prime_field_element_t N0, N1, S1, S2, zero = {0};
    prime_field_square(N0, input.re);
    prime_field_square(N1, input.im);
    prime_field_addition(S1, N0, N1);
    prime_field_inverse(S1, S1);
    prime_field_subtraction(S2, zero, input.im);
    prime_field_multiplication(output->re, S1, input.re);
    prime_field_multiplication(output->im, S1, S2);
}

void quadratic_field_negate(quadratic_field_element_t *output, quadratic_field_element_t input) {
    prime_field_negate(output->re, input.re);
    prime_field_negate(output->im, input.im);
}

void quadratic_field_conjugate(quadratic_field_element_t *output, quadratic_field_element_t input) {
    prime_field_copy(output->re, input.re);
    prime_field_negate(output->im, input.im);
}

void quadratic_field_divide_by_2(quadratic_field_element_t *output, quadratic_field_element_t input) {
    prime_field_divide_by_2(output->re, input.re);
    prime_field_divide_by_2(output->im, input.im);
}

void quadratic_field_to_montgomery_representation(quadratic_field_element_t *output, quadratic_field_element_t input) {
    prime_field_to_montgomery_representation(output->re, input.re);
    prime_field_to_montgomery_representation(output->im, input.im);
}

void
quadratic_field_from_montgomery_representation(quadratic_field_element_t *output, quadratic_field_element_t input) {
    prime_field_from_montgomery_representation(output->re, input.re);
    prime_field_from_montgomery_representation(output->im, input.im);
}

uint8_t quadratic_field_is_zero(quadratic_field_element_t input) {
    return prime_field_is_zero(input.re) & prime_field_is_zero(input.im);
}

uint8_t quadratic_field_is_equal(quadratic_field_element_t input_a, quadratic_field_element_t input_b) {
    return prime_field_is_equal(input_a.re, input_b.re) & prime_field_is_equal(input_a.im, input_b.im);
}

uint8_t quadratic_field_is_square(quadratic_field_element_t *output, quadratic_field_element_t input) {
    prime_field_element_t EXPONENT;
    quadratic_field_element_t input_1, alpha, alpha_conjugated, input_0, minus_one, x0, temp;

    quadratic_field_set_to_zero(&alpha_conjugated);
    quadratic_field_set_to_one(&minus_one);
    quadratic_field_negate(&minus_one, minus_one);

    int i, j;
    uint64_t flag;
    uint8_t c = 0;  // borrow
    // +++++++ To avoid pre-computation of (p - 3) / 4
    subtraction_with_borrow_u64(EXPONENT[0], c, 0x0, (PRIME_CHARACTERISTIC[0]), UINT64_C(0x3));
    for (i = 1; i < FIELD_64BITS_WORDS; i++) {
        subtraction_with_borrow_u64(EXPONENT[i], c, c, (PRIME_CHARACTERISTIC[i]), UINT64_C(0x0));
    }
    multiprecision_shift_to_right(EXPONENT, FIELD_64BITS_WORDS);
    multiprecision_shift_to_right(EXPONENT, FIELD_64BITS_WORDS);
    // +++++++
    // input_1 <- input ^ ([p - 3] / 4)
    quadratic_field_set_to_one(&input_1);
    quadratic_field_copy(&temp, input);
    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        flag = 1;
        for (j = 0; j < 64; j++) {
            if ((flag & EXPONENT[i]) != 0)
                quadratic_field_multiplication(&input_1, temp, input_1);
            quadratic_field_square(&temp, temp);
            flag <<= 1;
        }
    }

    quadratic_field_square(&alpha, input_1);
    quadratic_field_multiplication(&alpha, alpha, input);

    quadratic_field_conjugate(&alpha_conjugated, alpha);
    quadratic_field_multiplication(&input_0, alpha_conjugated, alpha);

    if (quadratic_field_is_equal(minus_one, input_0)) { return 0; }

    quadratic_field_multiplication(&x0, input_1, input);

    if (quadratic_field_is_equal(minus_one, alpha)) {
        prime_field_negate(output->re, x0.im);
        prime_field_copy(output->im, x0.re);
    } else {
        quadratic_field_subtraction(&alpha, alpha, minus_one);
        c = 0;  // borrow
        // +++++++ To avoid pre-computation of (p - 1) / 2
        subtraction_with_borrow_u64(EXPONENT[0], c, 0x0, (PRIME_CHARACTERISTIC[0]), UINT64_C(0x1));
        for (i = 1; i < FIELD_64BITS_WORDS; i++) {
            subtraction_with_borrow_u64(EXPONENT[i], c, c, (PRIME_CHARACTERISTIC[i]), UINT64_C(0x0));
        }
        multiprecision_shift_to_right(EXPONENT, FIELD_64BITS_WORDS);
        // +++++++
        // output <- alpha ^ ([p - 1] / 2)
        quadratic_field_set_to_one(output);
        quadratic_field_copy(&temp, alpha);
        for (i = 0; i < FIELD_64BITS_WORDS; i++) {
            flag = 1;
            for (j = 0; j < 64; j++) {
                if ((flag & EXPONENT[i]) != 0)
                    quadratic_field_multiplication(output, temp, *output);
                quadratic_field_square(&temp, temp);
                flag <<= 1;
            }
        }

        quadratic_field_multiplication(output, *output, x0);
    }
    return 1;
}

void quadratic_field_element_to_bytes(uint8_t *output, quadratic_field_element_t input) {
    quadratic_field_element_t t;

    quadratic_field_from_montgomery_representation(&t, input);
    memcpy(output, t.re, FIELD_BYTES);
    memcpy(&output[FIELD_BYTES], t.im, FIELD_BYTES);
}

void quadratic_field_element_from_bytes(quadratic_field_element_t *output, const uint8_t *input) {
    output->re[FIELD_64BITS_WORDS - 1] = 0;
    memcpy(output->re, input, FIELD_BYTES);
    output->im[FIELD_64BITS_WORDS - 1] = 0;
    memcpy(output->im, &input[FIELD_BYTES], FIELD_BYTES);
    quadratic_field_to_montgomery_representation(output, *output);
}
