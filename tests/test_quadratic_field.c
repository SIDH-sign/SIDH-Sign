#include "test_declarations.h"
#include "test_utils.h"
#include <prime_field.h>
#include <quadratic_field.h>

/*
 * Test cases
 */

static MunitResult addition_and_subtraction(MUNIT_UNUSED const MunitParameter params[],
                                            MUNIT_UNUSED void *user_data_or_fixture) {

    quadratic_field_element_t a, b, c, d, e, f;

    quadratic_field_random(&a);
    quadratic_field_set_to_zero(&b);
    quadratic_field_set_to_zero(&c);
    quadratic_field_set_to_zero(&d);

    assert_memory_not_equal(sizeof(quadratic_field_element_t), &a, &d);  // Random value different from zero

    quadratic_field_addition(&c, a, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &a, &c);      // a + 0 = a
    quadratic_field_set_to_zero(&c);
    quadratic_field_addition(&c, b, a);
    assert_memory_equal(sizeof(quadratic_field_element_t), &a, &c);      // 0 + a = a

    quadratic_field_negate(&b, a);
    assert_memory_not_equal(sizeof(quadratic_field_element_t), &a, &b);  // -a != a
    quadratic_field_addition(&c, a, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &c, &d);      // a - a = 0
    quadratic_field_addition(&c, b, a);
    assert_memory_equal(sizeof(quadratic_field_element_t), &c, &d);      // (-a) + a = 0

    quadratic_field_addition(&b, a, a);
    quadratic_field_divide_by_2(&c, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &c, &a);      // (2a)/2 = a

    quadratic_field_random(&b);
    assert_memory_not_equal(sizeof(quadratic_field_element_t), &b, &d);  // Random value different from zero

    quadratic_field_addition(&c, a, b);
    quadratic_field_subtraction(&d, a, b);
    assert_memory_not_equal(sizeof(quadratic_field_element_t), &b, &d);  // a + b != a - b

    quadratic_field_subtraction(&e, c, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &e, &a);      // (a + b) - b = a

    quadratic_field_addition(&f, d, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &f, &a);      // (a - b) + b = a

    quadratic_field_addition(&e, c, d);
    quadratic_field_divide_by_2(&e, e);
    assert_memory_equal(sizeof(quadratic_field_element_t), &e, &a);      // ([a + b] + [a - b])/2 = a

    quadratic_field_subtraction(&f, c, d);
    quadratic_field_divide_by_2(&f, f);
    assert_memory_equal(sizeof(quadratic_field_element_t), &f, &b);      // ([a + b] - [a - b])/2 = b

    quadratic_field_subtraction(&c, b, a);
    quadratic_field_negate(&c, c);
    assert_memory_equal(sizeof(quadratic_field_element_t), &c, &d);      // (a - b) = -(b - a)

    return MUNIT_OK;
}


static MunitResult multiplication_and_square(MUNIT_UNUSED const MunitParameter params[],
                                             MUNIT_UNUSED void *user_data_or_fixture) {

    quadratic_field_element_t a, b, c, d, e;

    quadratic_field_set_to_zero(&a);
    quadratic_field_set_to_zero(&b);
    quadratic_field_set_to_zero(&c);
    quadratic_field_set_to_zero(&d);
    quadratic_field_set_to_one(&e);

    a.re[0] = 1;
    quadratic_field_copy(&b, a);

    quadratic_field_to_montgomery_representation(&b, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &e);    // 1 -> R

    quadratic_field_from_montgomery_representation(&b, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &a);       // R -> 1

    quadratic_field_random(&a);
    quadratic_field_multiplication(&b, a, e);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &a);       // a * 1 = a

    quadratic_field_multiplication(&b, e, a);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &a);       // 1 * a = a

    quadratic_field_set_to_zero(&c);
    quadratic_field_multiplication(&b, a, c);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &c);       // a * 0 = 0

    quadratic_field_multiplication(&b, c, a);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &c);       // 0 * a = 0

    c.re[0] = 2;
    quadratic_field_to_montgomery_representation(&c, c);
    quadratic_field_addition(&b, a, a);
    quadratic_field_multiplication(&d, a, c);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &d);       // 2 * a = a + a

    quadratic_field_set_to_zero(&b);
    quadratic_field_set_to_zero(&c);
    quadratic_field_square(&b, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &c);       // 0² = 0

    quadratic_field_set_to_one(&b);
    quadratic_field_square(&b, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &e);       // 1² = 1

    quadratic_field_square(&b, a);
    quadratic_field_multiplication(&c, a, a);
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &c);       // a * a = a²

    return MUNIT_OK;
}


static MunitResult field_inverse(MUNIT_UNUSED const MunitParameter params[],
                                 MUNIT_UNUSED void *user_data_or_fixture) {

    quadratic_field_element_t a, b, c, one;

    quadratic_field_random(&a);
    quadratic_field_set_to_zero(&b);
    quadratic_field_set_to_zero(&c);
    quadratic_field_set_to_one(&one);

    assert_memory_not_equal(sizeof(quadratic_field_element_t), &a, &c);  // Random value different from zero

    quadratic_field_inverse(&b, a);
    quadratic_field_multiplication(&c, a, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &c, &one);     // a * a⁻¹ = 1

    quadratic_field_inverse(&b, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &a, &b);       // (a⁻¹)⁻¹ = a

    quadratic_field_set_to_one(&a);
    quadratic_field_inverse(&b, a);
    assert_memory_equal(sizeof(quadratic_field_element_t), &a, &b);       // (1⁻¹) = 1

    quadratic_field_multiplication(&c, a, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &c, &a);       // 1 * 1 = 1

    return MUNIT_OK;
}

static MunitResult field_is_square(MUNIT_UNUSED const MunitParameter params[],
                                   MUNIT_UNUSED void *user_data_or_fixture) {

    quadratic_field_element_t a, b, c;

    quadratic_field_random(&a);
    quadratic_field_square(&a, a);
    quadratic_field_set_to_zero(&b);
    quadratic_field_set_to_zero(&c);

//    assert_memory_not_equal(sizeof(quadratic_field_element_t), &a, &c);  // Random value different from zero
//
//    assert_true(quadratic_field_is_square(&b, a));
//    quadratic_field_square(&b, b);                  // testing with overwriting
//    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &a);     // c ^ 2 = a
//
//    assert_true(quadratic_field_is_square(&b, a));
//    quadratic_field_square(&c, b);
//    assert_memory_equal(sizeof(quadratic_field_element_t), &c, &a);     // c ^ 2 = a

    quadratic_field_set_to_zero(&b);
    prime_field_copy(b.re, a.re);
    assert_true(quadratic_field_is_square(&a, b));
    quadratic_field_square(&c, a);
    assert_memory_equal(sizeof(quadratic_field_element_t), &c, &b);     // c ^ 2 = b

    quadratic_field_set_to_one(&a);
    assert_true(quadratic_field_is_square(&b, a));
    quadratic_field_square(&c, b);
    assert_memory_equal(sizeof(quadratic_field_element_t), &c, &a);     // c ^ 2 = 1

    quadratic_field_set_to_zero(&a);
    assert_true(quadratic_field_is_square(&b, a));
    assert_memory_equal(sizeof(quadratic_field_element_t), &b, &a);     // b = 0

    return MUNIT_OK;
}

/*
 * Register test cases
 */

MunitTest test_quadratic_fields[] = {
        TEST_CASE(addition_and_subtraction),
        TEST_CASE(multiplication_and_square),
        TEST_CASE(field_inverse),
        TEST_CASE(field_is_square),
        TEST_END
};
