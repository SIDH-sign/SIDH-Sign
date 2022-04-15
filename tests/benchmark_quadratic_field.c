#include "test_declarations.h"
#include "test_utils.h"
#include <quadratic_field.h>

/*
 * Setup functions
 */

static quadratic_field_element_t a, b, am, bm, c, d, zero, one;

static void *setup_quadratic_field_element(MUNIT_UNUSED const MunitParameter params[],
                                       MUNIT_UNUSED void *user_data) {
    quadratic_field_random(&a);
    quadratic_field_random(&b);
    quadratic_field_set_to_zero(&c);
    quadratic_field_set_to_zero(&d);
    quadratic_field_set_to_zero(&zero);
    quadratic_field_set_to_one(&one);
    assert_memory_not_equal(FIELD_64BITS_WORDS, a.re, c.re);
    assert_memory_not_equal(FIELD_64BITS_WORDS, a.im, c.im);
    assert_memory_not_equal(FIELD_64BITS_WORDS, b.re, c.re);
    assert_memory_not_equal(FIELD_64BITS_WORDS, b.im, c.im);
    assert_memory_not_equal(FIELD_64BITS_WORDS, a.re, c.re);
    assert_memory_not_equal(FIELD_64BITS_WORDS, a.im, c.im);
    return NULL;
}

/*
 * Test cases
 */

static MunitResult add_zero(MUNIT_UNUSED const MunitParameter params[],
                            MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_addition(&c, a, zero);
    return MUNIT_OK;
}

static MunitResult add_one(MUNIT_UNUSED const MunitParameter params[],
                           MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_addition(&c, a, one);
    return MUNIT_OK;
}


static MunitResult add_rand(MUNIT_UNUSED const MunitParameter params[],
                            MUNIT_UNUSED void *user_data_or_fixture) {
    escape(&c);
    quadratic_field_addition(&c, a, b);
    return MUNIT_OK;
}


static MunitResult negate_rand(MUNIT_UNUSED const MunitParameter params[],
                               MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_negate(&c, a);
    return MUNIT_OK;
}


static MunitResult negate_add_rand(MUNIT_UNUSED const MunitParameter params[],
                                   MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_negate(&c, a);
    quadratic_field_addition(&d, c, b);
    return MUNIT_OK;
}


static MunitResult to_montgomery(MUNIT_UNUSED const MunitParameter params[],
                                 MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_to_montgomery_representation(&a, a);
    return MUNIT_OK;
}


static MunitResult mul_zero(MUNIT_UNUSED const MunitParameter params[],
                            MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_multiplication(&c, a, zero);
    return MUNIT_OK;
}


static MunitResult mul_one(MUNIT_UNUSED const MunitParameter params[],
                           MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_multiplication(&c, a, one);
    return MUNIT_OK;
}


static MunitResult mul_rand(MUNIT_UNUSED const MunitParameter params[],
                            MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_multiplication(&c, a, b);
    return MUNIT_OK;
}

static MunitResult square_zero(MUNIT_UNUSED const MunitParameter params[],
                               MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_square(&c, zero);
    return MUNIT_OK;
}

static MunitResult square_one(MUNIT_UNUSED const MunitParameter params[],
                              MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_square(&c, one);
    return MUNIT_OK;
}

static MunitResult square_rand(MUNIT_UNUSED const MunitParameter params[],
                               MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_square(&c, a);
    return MUNIT_OK;
}

static MunitResult inverse_one(MUNIT_UNUSED const MunitParameter params[],
                               MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_inverse(&c, one);
    return MUNIT_OK;
}

static MunitResult inverse_rand(MUNIT_UNUSED const MunitParameter params[],
                                MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_inverse(&c, a);
    return MUNIT_OK;
}

/*
 * Register test cases
 */

MunitTest benchmark_quadratic_fields[] = {
        TEST_CASE_SETUP(add_zero, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(add_one, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(add_rand, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(negate_rand, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(negate_add_rand, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(to_montgomery, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(mul_zero, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(mul_one, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(mul_rand, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(square_zero, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(square_one, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(square_rand, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(inverse_one, setup_quadratic_field_element, NULL),
        TEST_CASE_SETUP(inverse_rand, setup_quadratic_field_element, NULL),
        TEST_END
};
