#include "test_declarations.h"
#include "test_utils.h"
#include <elliptic_curve.h>

/*
 * Private auxiliary functions
 */


static void ladder_naive(x_only_point_t *output_P_plus_kQ,
                  quadratic_field_element_t input_P,
                  quadratic_field_element_t input_Q,
                  quadratic_field_element_t input_P_minus_Q,
                  const scalar_t input_k,
                  int input_bits_of_k,
                  projective_curve_alice_t A) {

    x_only_point_t R0, R1, R2;
    // Initializing points
    quadratic_field_copy(&R0.X, input_Q);
    quadratic_field_set_to_one(&R0.Z);
    quadratic_field_copy(&R2.X, input_P_minus_Q);
    quadratic_field_set_to_one(&R2.Z);
    quadratic_field_copy(&R1.X, input_P);
    quadratic_field_set_to_one(&R1.Z);

    int i, j, k = 0;
    for (i = 0; i < SUBGROUP_ORDER_64BITS_WORDS; i++) {
        for (j = 0; j < 64; j++) {
            if (input_k[i] & ((uint64_t) 1 << j)) {
                x_only_point_addition(&R1, R0, R1, R2);
            } else {
                x_only_point_addition(&R2, R0, R2, R1);
            }
            x_only_point_doubling(&R0, R0, A);
            k += 1;
            if (k == input_bits_of_k)
                break;
        }
        if (k == input_bits_of_k)
            break;
    }
    x_only_point_copy(output_P_plus_kQ, R1);
}


/*
 * Test cases
 */

static MunitResult curve_alice(MUNIT_UNUSED const MunitParameter params[],
                        MUNIT_UNUSED void *user_data_or_fixture) {

    x_only_point_t P = {0}, Q = {0}, P_minus_Q = {0}, T = {0}, R = {0};
    scalar_t k = {1}, z = {0};

    quadratic_field_element_t a = { .im = {0}, .re = {[0]=6}};
    quadratic_field_to_montgomery_representation(&a, a);

    projective_curve_alice_t curve_a;
    to_projective_curve_alice(&curve_a, a);

    projective_curve_bob_t curve_b;
    to_projective_curve_bob(&curve_b, a);

    // +++++++ order-2ᵃ point generators (Alice)
    initialize_generators_alice(&P.X, &Q.X, &P_minus_Q.X);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_set_to_one(&P_minus_Q.Z);
    // P
    x_only_multiple_point_doublings(&T, P, EXPONENT_ALICE - 1, curve_a);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_doubling(&T, T, curve_a);
    assert_true(x_only_point_is_infinity(T));
    x_only_multiple_point_triplings(&T, P, EXPONENT_BOB, curve_b);
    assert_false(x_only_point_is_infinity(T));
    // Q
    x_only_multiple_point_doublings(&T, Q, EXPONENT_ALICE - 1, curve_a);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_doubling(&T, T, curve_a);
    assert_true(x_only_point_is_infinity(T));
    x_only_multiple_point_triplings(&T, Q, EXPONENT_BOB, curve_b);
    assert_false(x_only_point_is_infinity(T));
    // P-Q
    x_only_multiple_point_doublings(&T, P_minus_Q, EXPONENT_ALICE - 1, curve_a);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_doubling(&T, T, curve_a);
    assert_true(x_only_point_is_infinity(T));
    x_only_multiple_point_triplings(&T, P_minus_Q, EXPONENT_BOB, curve_b);
    assert_false(x_only_point_is_infinity(T));

    k[0] = 1;
    x_only_point_addition(&T, P, Q, P_minus_Q);
    x_only_three_point_ladder(&R,
                              P.X, Q.X, P_minus_Q.X,
                              k, SUBGROUP_ORDER_BITS_ALICE,
                              a);
    assert_true(x_only_point_is_equal(T, R));
    k[0] = 0;
    x_only_three_point_ladder(&R,
                              P.X, Q.X, P_minus_Q.X,
                              k, SUBGROUP_ORDER_BITS_ALICE,
                              a);
    assert_true(x_only_point_is_equal(P, R));

    k[0] = 0;
    random_bob_private_key(k);
    assert_memory_not_equal(sizeof(scalar_t), k, z);
    ladder_naive(&T,
                 P.X, Q.X, P_minus_Q.X,
                 k, SUBGROUP_ORDER_BITS_ALICE,
                 curve_a);
    x_only_three_point_ladder(&R,
                              P.X, Q.X, P_minus_Q.X,
                              k, SUBGROUP_ORDER_BITS_ALICE,
                              a);
    assert_true(x_only_point_is_equal(T, R));

    return MUNIT_OK;
}


static MunitResult curve_bob(MUNIT_UNUSED const MunitParameter params[],
                      MUNIT_UNUSED void *user_data_or_fixture) {
    x_only_point_t P = {0}, Q = {0}, P_minus_Q = {0}, T = {0}, R = {0};
    scalar_t k = {1}, z = {0};

    quadratic_field_element_t a = { .im = {0}, .re = {[0]=6}};
    quadratic_field_to_montgomery_representation(&a, a);

    projective_curve_alice_t curve_a;
    to_projective_curve_alice(&curve_a, a);

    projective_curve_bob_t curve_b;
    to_projective_curve_bob(&curve_b, a);

    // +++++++ order-3ᵇ point generators (Bob)
    initialize_generators_bob(&P.X, &Q.X, &P_minus_Q.X);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_set_to_one(&P_minus_Q.Z);
    // P
    x_only_multiple_point_triplings(&T, P, EXPONENT_BOB - 1, curve_b);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_tripling(&T, T, curve_b);
    assert_true(x_only_point_is_infinity(T));
    x_only_multiple_point_doublings(&T, P, EXPONENT_ALICE, curve_a);
    assert_false(x_only_point_is_infinity(T));
    // Q
    x_only_multiple_point_triplings(&T, Q, EXPONENT_BOB - 1, curve_b);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_tripling(&T, T, curve_b);
    assert_true(x_only_point_is_infinity(T));
    x_only_multiple_point_doublings(&T, Q, EXPONENT_ALICE, curve_a);
    assert_false(x_only_point_is_infinity(T));
    // P-Q
    x_only_multiple_point_triplings(&T, P_minus_Q, EXPONENT_BOB - 1, curve_b);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_tripling(&T, T, curve_b);
    assert_true(x_only_point_is_infinity(T));
    x_only_multiple_point_doublings(&T, P_minus_Q, EXPONENT_ALICE, curve_a);
    assert_false(x_only_point_is_infinity(T));

    k[0] = 1;
    x_only_point_addition(&T, P, Q, P_minus_Q);
    x_only_three_point_ladder(&R,
                              P.X, Q.X, P_minus_Q.X,
                              k, SUBGROUP_ORDER_BITS_BOB,
                              a);
    assert_true(x_only_point_is_equal(T, R));
    k[0] = 0;
    x_only_three_point_ladder(&R,
                              P.X, Q.X, P_minus_Q.X,
                              k, SUBGROUP_ORDER_BITS_BOB,
                              a);
    assert_true(x_only_point_is_equal(P, R));

    k[0] = 0;
    random_bob_private_key(k);
    assert_memory_not_equal(sizeof(scalar_t), k, z);
    ladder_naive(&T,
                 P.X, Q.X, P_minus_Q.X,
                 k, SUBGROUP_ORDER_BITS_BOB,
                 curve_a);
    x_only_three_point_ladder(&R,
                              P.X, Q.X, P_minus_Q.X,
                              k, SUBGROUP_ORDER_BITS_BOB,
                              a);
    assert_true(x_only_point_is_equal(T, R));

    return MUNIT_OK;
}


/*
 * Register test cases
 */

MunitTest test_ecc[] = {
        TEST_CASE(curve_alice),
        TEST_CASE(curve_bob),
        TEST_END
};
