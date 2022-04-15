#include <prime_field.h>
#include "test_declarations.h"
#include "test_utils.h"

static MunitResult field_sampling(MUNIT_UNUSED const MunitParameter params[],
                                  MUNIT_UNUSED void *user_data_or_fixture) {
    prime_field_element_t seed, rnd, aux, one;
    uint8_t value_equality, expected_value;
    uint32_t num_checks = 10000;
    RANDOM_PRIME_FIELD_ELEMENT(seed);
    prime_field_set_to_zero(rnd);
    prime_field_set_to_zero(aux);
    prime_field_set_to_one(one);

    assert_memory_not_equal(FIELD_BYTES, seed, rnd);  // Random value different from zero

    prime_field_copy(aux, PRIME_CHARACTERISTIC);
    assert_memory_equal(FIELD_BYTES, aux, PRIME_CHARACTERISTIC);

    aux[FIELD_64BITS_WORDS - 1]++;
    value_equality = prime_field_is_smaller(aux, PRIME_CHARACTERISTIC);
    expected_value = 0x00;
    assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);

    aux[FIELD_64BITS_WORDS - 1] -= 2;
    value_equality = prime_field_is_smaller(aux, PRIME_CHARACTERISTIC);
    expected_value = 0x01;
    assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);


    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    for (uint32_t i = 0; i < num_checks; i++) {
        prime_field_sample(rnd, &shake_st);
        value_equality = prime_field_is_smaller(rnd, PRIME_CHARACTERISTIC);
        assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);
    }


    return MUNIT_OK;
}


static MunitResult addition_and_subtraction(MUNIT_UNUSED const MunitParameter params[],
                                            MUNIT_UNUSED void *user_data_or_fixture) {
    prime_field_element_t a, b, c, d, e, f;

    RANDOM_PRIME_FIELD_ELEMENT(a);
    prime_field_set_to_zero(b);
    prime_field_set_to_zero(c);
    prime_field_set_to_zero(d);

    assert_memory_not_equal(FIELD_64BITS_WORDS, a, d);  // Random value different from zero

    prime_field_addition(c, a, b);
    assert_memory_equal(FIELD_64BITS_WORDS, a, c);      // a + 0 = a
    prime_field_set_to_zero(c);
    prime_field_addition(c, b, a);
    assert_memory_equal(FIELD_64BITS_WORDS, a, c);      // 0 + a = a

    prime_field_negate(b, a);
    assert_memory_not_equal(FIELD_64BITS_WORDS, a, b);  // -a != a
    prime_field_addition(c, a, b);
    assert_memory_equal(FIELD_64BITS_WORDS, c, d);      // a - a = 0
    prime_field_addition(c, b, a);
    assert_memory_equal(FIELD_64BITS_WORDS, c, d);      // (-a) + a = 0

    prime_field_addition(b, a, a);
    prime_field_divide_by_2(c, b);
    assert_memory_equal(FIELD_64BITS_WORDS, c, a);      // (2a)/2 = a

    RANDOM_PRIME_FIELD_ELEMENT(b);
    assert_memory_not_equal(FIELD_64BITS_WORDS, b, d);  // Random value different from zero

    prime_field_addition(c, a, b);
    prime_field_subtraction(d, a, b);
    assert_memory_not_equal(FIELD_64BITS_WORDS, b, d);  // a + b != a - b

    prime_field_subtraction(e, c, b);
    assert_memory_equal(FIELD_64BITS_WORDS, e, a);      // (a + b) - b = a

    prime_field_addition(f, d, b);
    assert_memory_equal(FIELD_64BITS_WORDS, f, a);      // (a - b) + b = a

    prime_field_addition(e, c, d);
    prime_field_divide_by_2(e, e);
    assert_memory_equal(FIELD_64BITS_WORDS, e, a);      // ([a + b] + [a - b])/2 = a

    prime_field_subtraction(f, c, d);
    prime_field_divide_by_2(f, f);
    assert_memory_equal(FIELD_64BITS_WORDS, f, b);      // ([a + b] - [a - b])/2 = b

    prime_field_subtraction(c, b, a);
    prime_field_negate(c, c);
    assert_memory_equal(FIELD_64BITS_WORDS, c, d);      // (a - b) = -(b - a)

    return MUNIT_OK;
}


static MunitResult multiplication_and_square(MUNIT_UNUSED const MunitParameter params[],
                                             MUNIT_UNUSED void *user_data_or_fixture) {
    prime_field_element_t a, b, c, d;

    prime_field_set_to_zero(a);
    prime_field_set_to_zero(b);
    prime_field_set_to_zero(c);
    prime_field_set_to_zero(d);

    a[0] = 1;
    prime_field_copy(b, a);

    prime_field_to_montgomery_representation(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, b, MONTGOMERY_CONSTANT_ONE);    // 1 -> R

    prime_field_from_montgomery_representation(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, b, a);                          // R -> 1

    RANDOM_PRIME_FIELD_ELEMENT(a);
    prime_field_multiplication(b, a, MONTGOMERY_CONSTANT_ONE);
    assert_memory_equal(FIELD_64BITS_WORDS, b, a);                          // a * 1 = a

    prime_field_multiplication(b, MONTGOMERY_CONSTANT_ONE, a);
    assert_memory_equal(FIELD_64BITS_WORDS, b, a);                          // 1 * a = a

    prime_field_set_to_zero(c);
    prime_field_multiplication(b, a, c);
    assert_memory_equal(FIELD_64BITS_WORDS, b, c);                          // a * 0 = 0

    prime_field_multiplication(b, c, a);
    assert_memory_equal(FIELD_64BITS_WORDS, b, c);                          // 0 * a = 0

    c[0] = 2;
    prime_field_to_montgomery_representation(c, c);
    prime_field_addition(b, a, a);
    prime_field_multiplication(d, a, c);
    assert_memory_equal(FIELD_64BITS_WORDS, b, d);                          // 2 * a = a + a

    prime_field_set_to_zero(b);
    prime_field_set_to_zero(c);
    prime_field_square(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, b, c);                          // 0^2 = 0

    prime_field_set_to_one(b);
    prime_field_square(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, b, MONTGOMERY_CONSTANT_ONE);    // 1^2 = 1

    prime_field_square(b, a);
    prime_field_multiplication(c, a, a);
    assert_memory_equal(FIELD_64BITS_WORDS, b, c);                          // a * a = a^2

    return MUNIT_OK;
}


static MunitResult field_inverse(MUNIT_UNUSED const MunitParameter params[],
                                 MUNIT_UNUSED void *user_data_or_fixture) {

    prime_field_element_t a, b, c;

    RANDOM_PRIME_FIELD_ELEMENT(a);
    prime_field_set_to_zero(b);
    prime_field_set_to_zero(c);

    assert_memory_not_equal(FIELD_64BITS_WORDS, a, c);  // Random value different from zero

    prime_field_set_to_zero(b);
    assert_memory_not_equal(FIELD_64BITS_WORDS, a, b);

    prime_field_inverse(b, a);
    prime_field_multiplication(c, a, b);
    assert_memory_equal(FIELD_64BITS_WORDS, c, MONTGOMERY_CONSTANT_ONE);    // a * a⁻¹ = 1

    prime_field_inverse(b, b);
    assert_memory_equal(FIELD_64BITS_WORDS, a, b);                          // (a⁻¹)⁻¹ = a

    prime_field_set_to_one(a);
    prime_field_inverse(b, a);
    assert_memory_equal(FIELD_64BITS_WORDS, a, b);                          // (1⁻¹) = 1

    prime_field_multiplication(c, a, b);
    assert_memory_equal(FIELD_64BITS_WORDS, c, MONTGOMERY_CONSTANT_ONE);    // 1 * 1 = 1

    return MUNIT_OK;
}

/*
 * Register test cases
 */

MunitTest test_prime_fields[] = {
        TEST_CASE(field_sampling),
        TEST_CASE(addition_and_subtraction),
        TEST_CASE(multiplication_and_square),
        TEST_CASE(field_inverse),
        TEST_END
};

