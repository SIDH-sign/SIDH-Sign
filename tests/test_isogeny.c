#include "test_declarations.h"
#include "test_utils.h"
#include <isogeny.h>


/*
 * Test cases
 */

static MunitResult test_isogeny_alice(MUNIT_UNUSED const MunitParameter params[],
                                      MUNIT_UNUSED void *user_data_or_fixture) {

    x_only_point_t P = {0}, Q = {0}, P_minus_Q = {0}, R = {0}, K = {0}, T = {0};
    scalar_t k = {1}, z = {0};

    quadratic_field_element_t a = { .im = {0}, .re = {[0]=6}};
    quadratic_field_element_t isogeny_coefficients[3];
    quadratic_field_to_montgomery_representation(&a, a);

    projective_curve_alice_t curve_a;
    to_projective_curve_alice(&curve_a, a);

    // +++++++ order-2ᵃ point generators (Alice)
    initialize_generators_alice(&P.X, &Q.X, &P_minus_Q.X);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_set_to_one(&P_minus_Q.Z);

    // +++++++ 4-isogeny test
    random_bob_private_key(k);
    assert_memory_not_equal(sizeof(scalar_t), k, z);

    x_only_three_point_ladder(&R, P.X, Q.X, P_minus_Q.X, k, SUBGROUP_ORDER_BITS_ALICE, a);
    assert_false(x_only_point_is_infinity(R));

    x_only_multiple_point_doublings(&K, R, EXPONENT_ALICE - 2, curve_a);
    assert_false(x_only_point_is_infinity(K));

    x_only_4_isogeny(&curve_a, isogeny_coefficients, K);

    // Image of K must be the point at infinity
    x_only_4_isogeny_evaluation(&K, K, isogeny_coefficients);
    assert_true(x_only_point_is_infinity(K));

    // Recall, K = P + [k]Q so image of Q is an order-2ᵃ point
    x_only_4_isogeny_evaluation(&Q, Q, isogeny_coefficients);
    assert_false(x_only_point_is_infinity(Q));
    x_only_multiple_point_doublings(&T, Q, EXPONENT_ALICE - 2, curve_a);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_doubling(&T, T, curve_a);
    x_only_point_doubling(&T, T, curve_a);
    assert_true(x_only_point_is_infinity(T));

    // Image of R is an order-2ᵃ⁻² point
    x_only_4_isogeny_evaluation(&R, R, isogeny_coefficients);
    assert_false(x_only_point_is_infinity(R));
    x_only_multiple_point_doublings(&K, R, EXPONENT_ALICE - 3, curve_a);
    assert_false(x_only_point_is_infinity(K));
    x_only_point_doubling(&T, K, curve_a);
    assert_true(x_only_point_is_infinity(T));

#if(EXPONENT_ALICE % 2 == 1)
    // +++++++ 2-isogeny test
    assert_false(x_only_point_is_infinity(K));
    x_only_2_isogeny(&curve_a, K);

    // Recall, K = P + [k]Q so image of Q is an order-2ᵃ point
    x_only_2_isogeny_evaluation(&Q, Q, K);
    assert_false(x_only_point_is_infinity(Q));
    x_only_multiple_point_doublings(&T, Q, EXPONENT_ALICE - 1, curve_a);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_doubling(&T, T, curve_a);
    assert_true(x_only_point_is_infinity(T));

    // Image of R is an order-2ᵃ⁻³ point
    x_only_2_isogeny_evaluation(&R, R, K);
    assert_false(x_only_point_is_infinity(R));
    x_only_multiple_point_doublings(&T, R, EXPONENT_ALICE - 4, curve_a);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_doubling(&T, T, curve_a);
    assert_true(x_only_point_is_infinity(T));

    // Image of K must be the point at infinity
    x_only_2_isogeny_evaluation(&K, K, K);
    assert_true(x_only_point_is_infinity(K));
#endif

    return MUNIT_OK;
}


static MunitResult test_isogeny_bob(MUNIT_UNUSED const MunitParameter params[],
                                    MUNIT_UNUSED void *user_data_or_fixture) {
    x_only_point_t P = {0}, Q = {0}, P_minus_Q = {0}, R = {0}, K = {0}, T = {0};
    scalar_t k = {1}, z = {0};

    quadratic_field_element_t a = { .im = {0}, .re = {[0]=6}};
    quadratic_field_element_t  isogeny_coefficients[2];
    quadratic_field_to_montgomery_representation(&a, a);

    projective_curve_bob_t curve_b;
    to_projective_curve_bob(&curve_b, a);

    // +++++++ order-3ᵇ point generators (Bob)
    initialize_generators_bob(&P.X, &Q.X, &P_minus_Q.X);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_set_to_one(&P_minus_Q.Z);

    // +++++++ 3-isogeny test
    random_bob_private_key(k);
    assert_memory_not_equal(sizeof(scalar_t), k, z);

    x_only_three_point_ladder(&R, P.X, Q.X, P_minus_Q.X, k, SUBGROUP_ORDER_BITS_BOB, a);
    assert_false(x_only_point_is_infinity(R));

    x_only_multiple_point_triplings(&K, R, EXPONENT_BOB - 1, curve_b);
    assert_false(x_only_point_is_infinity(K));

    x_only_3_isogeny(&curve_b, isogeny_coefficients, K);

    // Image of K must be the point at infinity
    x_only_3_isogeny_evaluation(&K, K, isogeny_coefficients);
    assert_true(x_only_point_is_infinity(K));

    // Recall, K = P + [k]Q so image of Q is an order-3ᵇ point
    x_only_3_isogeny_evaluation(&Q, Q, isogeny_coefficients);
    assert_false(x_only_point_is_infinity(Q));
    x_only_multiple_point_triplings(&T, Q, EXPONENT_BOB - 1, curve_b);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_tripling(&T, T, curve_b);
    assert_true(x_only_point_is_infinity(T));

    // Image of R is an order-3ᵇ⁻¹ point
    x_only_3_isogeny_evaluation(&R, R, isogeny_coefficients);
    assert_false(x_only_point_is_infinity(R));
    x_only_multiple_point_triplings(&K, R, EXPONENT_BOB - 2, curve_b);
    assert_false(x_only_point_is_infinity(K));
    x_only_point_tripling(&T, K, curve_b);
    assert_true(x_only_point_is_infinity(T));

    return MUNIT_OK;
}


/*
 * Register test cases
 */
MunitTest test_isogeny[] = {
        TEST_CASE(test_isogeny_alice),
        TEST_CASE(test_isogeny_bob),
        TEST_END
};
