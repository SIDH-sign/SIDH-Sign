//
// Prime field arithmetic GF(p)
//

#include <string.h>
#include "prime_field.h"
#include "utilities.h"

void prime_field_copy(prime_field_element_t output, const prime_field_element_t input) {
    memcpy(output, input, sizeof(prime_field_element_t));
}

void prime_field_set_to_zero(prime_field_element_t input_output) {
    memset(input_output, 0, sizeof(prime_field_element_t));
}


void prime_field_sample(prime_field_element_t output, keccak_state *state) {
    shake256_squeeze((uint8_t *) output, FIELD_BYTES, state);
    output[FIELD_64BITS_WORDS - 1] &= MASK_FIELD_ELEMENT;
    while (!multiprecision_is_smaller(output, PRIME_CHARACTERISTIC, FIELD_64BITS_WORDS)) {
        shake256_squeeze((uint8_t *) output, FIELD_BYTES, state);
        output[FIELD_64BITS_WORDS - 1] &= MASK_FIELD_ELEMENT;
    }
}

void prime_field_set_to_one(prime_field_element_t input_output) {
    prime_field_copy(input_output, MONTGOMERY_CONSTANT_ONE);
}

// We should use Bernstein & Yang algorithm
void prime_field_inverse(prime_field_element_t output, const prime_field_element_t input) {

    prime_field_element_t temp, out;
    prime_field_element_t PRIME_CHARACTERISTIC_MINUS_2;
    prime_field_set_to_one(out);
    prime_field_copy(temp, input);

    int i, j;
    uint64_t flag;
    uint8_t b = 0;

    // +++++++ To avoid pre-computation of (p - 2)
    subtraction_with_borrow_u64(PRIME_CHARACTERISTIC_MINUS_2[0], b, 0x0, (PRIME_CHARACTERISTIC[0]), UINT64_C(0x2));
    for (i = 1; i < FIELD_64BITS_WORDS; i++) subtraction_with_borrow_u64(PRIME_CHARACTERISTIC_MINUS_2[i], b, b,
                                                                         (PRIME_CHARACTERISTIC[i]), UINT64_C(0x0));
    // +++++++

    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        flag = 1;
        for (j = 0; j < 64; j++) {
            if ((flag & PRIME_CHARACTERISTIC_MINUS_2[i]) != 0)
                prime_field_multiplication(out, temp, out);
            prime_field_square(temp, temp);
            flag <<= 1;
        }
    }
    prime_field_copy(output, out);
}

void prime_field_divide_by_2(prime_field_element_t output, const prime_field_element_t input) {
    uint8_t carry = 0;
    int i;
    uint64_t mask;

    mask = 0 - (uint64_t) (input[0] & 1);
    // If a is odd compute a + p
    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        addition_with_carry_u64(output[i], carry, carry, input[i], PRIME_CHARACTERISTIC[i] & mask);
    }

    multiprecision_shift_to_right(output, FIELD_64BITS_WORDS);
}

void prime_field_to_montgomery_representation(prime_field_element_t output, const prime_field_element_t input) {
    prime_field_multiplication(output, input, (uint64_t *) &MONTGOMERY_CONSTANT_R_SQUARED);
}

void prime_field_from_montgomery_representation(prime_field_element_t output, const prime_field_element_t input) {
    prime_field_element_t one = {0};
    one[0] = 1;
    prime_field_multiplication(output, input, one);
}

// constant-time check of a == 0
uint8_t prime_field_is_zero(const prime_field_element_t input) {
    int i;
    uint64_t output = 0;
    for (i = FIELD_64BITS_WORDS - 1; i >= 0; i--)
        output |= input[i];

    return (uint8_t) (output == 0);
}

uint8_t prime_field_is_equal(const prime_field_element_t input_a, const prime_field_element_t input_b) {
    int i;
    uint8_t r = 0;
    uint64_t t;

    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        t = 0;
        uint8_t *ta = (uint8_t *) &input_a[i];
        uint8_t *tb = (uint8_t *) &input_b[i];
        t = (ta[0] ^ tb[0]) | (ta[1] ^ tb[1]) | (ta[2] ^ tb[2]) | (ta[3] ^ tb[3]) |
            (ta[4] ^ tb[4]) | (ta[5] ^ tb[5]) | (ta[6] ^ tb[6]) | (ta[7] ^ tb[7]);

        t = (-t);
        t = t >> 63;
        r |= t;
    }

    return (uint8_t) (1 - r);
}

uint8_t prime_field_is_smaller(const prime_field_element_t input1, const prime_field_element_t input2) {
    uint64_t ret = 0, tmp, condition;
    for (uint64_t i = 0; i < FIELD_64BITS_WORDS; i++) {
        tmp = input1[i] ^ input2[i];
        condition = input1[i] - input2[i];
        condition ^= tmp & (condition ^ input1[i]);
        condition >>= 63;
        ret |= condition;
    }
    return ret;
}

uint8_t prime_field_is_square(const prime_field_element_t input) {
    prime_field_element_t EXPONENT, temp, input_1;
    uint64_t ret = 0, flag;
    uint8_t c = 0;  // borrow
    int i, j;
    // +++++++ To avoid pre-computation of (p - 1) / 2
    subtraction_with_borrow_u64(EXPONENT[0], c, 0x0, (PRIME_CHARACTERISTIC[0]), UINT64_C(0x1));
    for (i = 1; i < FIELD_64BITS_WORDS; i++) {
        subtraction_with_borrow_u64(EXPONENT[i], c, c, (PRIME_CHARACTERISTIC[i]), UINT64_C(0x0));
    }
    multiprecision_shift_to_right(EXPONENT, FIELD_64BITS_WORDS);
    // +++++++
    // input_1 <- input ^ ([p - 1] / 2)
    prime_field_set_to_one(input_1);
    prime_field_copy(temp, input);
    for (i = 0; i < FIELD_64BITS_WORDS; i++) {
        flag = 1;
        for (j = 0; j < 64; j++) {
            if ((flag & EXPONENT[i]) != 0)
                prime_field_multiplication(input_1, temp, input_1);
            prime_field_square(temp, temp);
            flag <<= 1;
        }
    }

    return prime_field_is_equal(input_1, MONTGOMERY_CONSTANT_ONE);
}
