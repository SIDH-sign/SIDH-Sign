#include "test_declarations.h"
#include "test_utils.h"
#include "utilities.h"
#include <pok.h>
#include <stdio.h>

/*
 * Test cases
 */

static MunitResult test_elligator2(MUNIT_UNUSED const MunitParameter params[],
                                   MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_element_t x, y, t, A, u, r_squared;
    x_only_point_t P;
    projective_curve_alice_t curve_alice;
    projective_curve_bob_t curve_bob;
    sidh_private_key_t sk_alice;
    sidh_public_key_t pk_alice;

    //Seed generation
    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    // Random supersingular Montgomery curve
    sidh_generate_private_key_alice(&sk_alice, &shake_st);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
    get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);

    // r²
    quadratic_field_random(&r_squared);
    quadratic_field_square(&r_squared, r_squared);
    // looking for a non quadratic residue
    prime_field_set_to_one(u.re);
    prime_field_set_to_one(u.im);
    while (quadratic_field_is_square(&t, u)) {
        prime_field_addition(u.re, u.re, u.im);
    }
    assert_false(quadratic_field_is_square(&t, u));

    assert_true(x_only_elligator2(&P, u, r_squared, A) <= 1);
    assert_false(x_only_point_is_infinity(P));

    // x <- X/Z
    quadratic_field_inverse(&t, P.Z);
    quadratic_field_multiplication(&x, t, P.X);
    // t <- x²
    quadratic_field_square(&t, x);
    // y <- Ax²
    quadratic_field_multiplication(&y, A, t);
    // y <- Ax² + x
    quadratic_field_addition(&y, y, x);
    // t <- x³
    quadratic_field_multiplication(&t, t, x);
    // y <- x³ + Ax² + x
    quadratic_field_addition(&y, y, t);
    assert_true(quadratic_field_is_square(&t, y));

    to_projective_curve_alice(&curve_alice, A);
    x_only_multiple_point_doublings(&P, P, EXPONENT_ALICE, curve_alice);
    to_projective_curve_bob(&curve_bob, A);
    x_only_multiple_point_triplings(&P, P, EXPONENT_BOB, curve_bob);
    assert_true(x_only_point_is_infinity(P));

    return MUNIT_OK;
}

static MunitResult test_is_full_order_bob(MUNIT_UNUSED const MunitParameter params[],
                                          MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_element_t A;
    x_only_point_t P, P3;
    projective_curve_bob_t curve;
    sidh_private_key_t sk_alice;
    sidh_public_key_t pk_alice;

    //Seed generation
    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    // Random supersingular Montgomery curve
    sidh_generate_private_key_alice(&sk_alice, &shake_st);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
    get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);
    to_projective_curve_bob(&curve, A);

    quadratic_field_copy(&P.X, pk_alice.P);
    quadratic_field_set_to_one(&P.Z);
    assert_true(x_only_is_full_order_bob(&P3, &P, A));
    x_only_point_tripling(&P3, P3, curve);
    assert_true(x_only_point_is_infinity(P3));

    quadratic_field_copy(&P.X, pk_alice.Q);
    quadratic_field_set_to_one(&P.Z);
    assert_true(x_only_is_full_order_bob(&P3, &P, A));
    x_only_point_tripling(&P3, P3, curve);
    assert_true(x_only_point_is_infinity(P3));

    quadratic_field_copy(&P.X, pk_alice.P_minus_Q);
    quadratic_field_set_to_one(&P.Z);
    assert_true(x_only_is_full_order_bob(&P3, &P, A));
    x_only_point_tripling(&P3, P3, curve);
    assert_true(x_only_point_is_infinity(P3));

    return MUNIT_OK;
}

static MunitResult test_canonical_basis_bob(MUNIT_UNUSED const MunitParameter params[],
                                            MUNIT_UNUSED void *user_data_or_fixture) {
    scalar_t k, z = {0};
    quadratic_field_element_t A, A_prime, u, t;
    x_only_point_t P, P3, Q, Q3, P_minus_Q, P3_minus_Q3, T;
    sidh_private_key_t sk_alice;
    sidh_public_key_t pk_alice;

    projective_curve_bob_t curve_b;

    //Seed generation
    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    // Random supersingular Montgomery curve
    sidh_generate_private_key_alice(&sk_alice, &shake_st);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
    get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);

    prime_field_set_to_one(u.re);
    prime_field_set_to_one(u.im);
    while (quadratic_field_is_square(&t, u)) {
        prime_field_addition(u.re, u.re, u.im);
    }
    assert_false(quadratic_field_is_square(&t, u));

    assert_true(x_only_canonical_basis_bob(&P, &Q, &P_minus_Q, u, A) == EXIT_SUCCESS);

    simultaneous_three_inverses(&P.Z, &Q.Z, &P_minus_Q.Z);
    quadratic_field_multiplication(&P.X, P.X, P.Z);
    quadratic_field_set_to_one(&P.Z);
    quadratic_field_multiplication(&Q.X, Q.X, Q.Z);
    quadratic_field_set_to_one(&Q.Z);
    quadratic_field_multiplication(&P_minus_Q.X, P_minus_Q.X, P_minus_Q.Z);
    quadratic_field_set_to_one(&P_minus_Q.Z);

    get_curve_from_points(&A_prime, P.X, Q.X, P_minus_Q.X);
    assert_memory_equal(sizeof(quadratic_field_element_t), &A, &A_prime);

    x_only_point_copy(&T, P);
    assert_true(x_only_is_full_order_bob(&P3, &T, A));
    x_only_point_copy(&T, Q);
    assert_true(x_only_is_full_order_bob(&Q3, &T, A));
    x_only_point_copy(&T, P_minus_Q);
    assert_true(x_only_is_full_order_bob(&P3_minus_Q3, &T, A));

    // +++++++ Testing correctness of P - Q
    random_bob_private_key(k);
    assert_memory_not_equal(sizeof(scalar_t), k, z);
    x_only_three_point_ladder(&T,
                              P.X, Q.X, P_minus_Q.X,
                              k, SUBGROUP_ORDER_BITS_BOB,
                              A);

    to_projective_curve_bob(&curve_b, A);
    x_only_multiple_point_triplings(&T, T, EXPONENT_BOB - 1, curve_b);
    assert_false(x_only_point_is_infinity(T));
    x_only_point_tripling(&T, T, curve_b);
    assert_true(x_only_point_is_infinity(T));

    return MUNIT_OK;
}

static int iter = 0;

static MunitResult test_canonical_basisx(MUNIT_UNUSED const MunitParameter params[],
                                        MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_element_t A, A_prime;
    sidh_private_key_t sk_alice;
    sidh_public_key_t pk_alice, pk = {0}, pk_prime = {0};

    iter++;

    //Seed generation
    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    // Random supersingular Montgomery curve
    sidh_generate_private_key_alice(&sk_alice, &shake_st);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
    get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);

    if (canonical_basis(&pk, A) != EXIT_SUCCESS) {
        printf("\niteration %i\n",iter);
        return MUNIT_ERROR;
    }
    assert_memory_not_equal(sizeof(sidh_public_key_t), &pk_alice, &pk);

    get_curve_from_points(&A_prime, pk.P, pk.Q, pk.P_minus_Q);
    assert_memory_equal(sizeof(quadratic_field_element_t), &A, &A_prime);

    assert_true(canonical_basis(&pk_prime, A) == EXIT_SUCCESS);
    assert_memory_equal(sizeof(sidh_public_key_t), &pk, &pk_prime);

    return MUNIT_OK;
}

static MunitResult test_two_dimensional_ladder(MUNIT_UNUSED const MunitParameter params[],
                                               MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_element_t A;
    sidh_private_key_t sk_alice;
    sidh_public_key_t pk_alice, pk = {0};
    x_only_point_t S, T;

    scalar_t k, z;
    memset(k, 0, sizeof(scalar_t));
    memset(z, 0, sizeof(scalar_t));
    z[0] = 1;


    //Seed generation
    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    // Random supersingular Montgomery curve
    sidh_generate_private_key_alice(&sk_alice, &shake_st);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
    get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);

    random_bob_private_key(k);
    x_only_three_point_ladder(&S,
                              pk_alice.P,
                              pk_alice.Q,
                              pk_alice.P_minus_Q,
                              k,
                              SUBGROUP_ORDER_BITS_BOB,
                              A);
    two_dimensional_scalar_multiplication_bob(&T,
                                              pk.P,
                                              pk.Q,
                                              pk.P_minus_Q,
                                              z,
                                              k,
                                              A);
    assert_true(x_only_point_is_equal(S, T));

    random_bob_private_key(k);
    x_only_three_point_ladder(&S,
                              pk_alice.Q,
                              pk_alice.P,
                              pk_alice.P_minus_Q,
                              k,
                              SUBGROUP_ORDER_BITS_BOB,
                              A);
    two_dimensional_scalar_multiplication_bob(&T,
                                              pk.P,
                                              pk.Q,
                                              pk.P_minus_Q,
                                              k,
                                              z,
                                              A);
    assert_true(x_only_point_is_equal(S, T));

    return MUNIT_OK;
}

static MunitResult test_two_dimensional_pohlig_hellman(MUNIT_UNUSED const MunitParameter params[],
                                                       MUNIT_UNUSED void *user_data_or_fixture) {
    quadratic_field_element_t A, yP, yQ, zQ, t, y, x;
    sidh_private_key_t sk_alice;
    sidh_public_key_t pk_alice, pk = {0};
    x_only_point_t K, R, T;

    projective_point_t P_twisted, Q_twisted, K_twisted;
    twisted_edwards_curve_t curve_edwards;
    scalar_t c, d, z;
    memset(z, 0, sizeof(scalar_t));


    //Seed generation
    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    // Random supersingular Montgomery curve
    sidh_generate_private_key_alice(&sk_alice, &shake_st);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
    get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);

    assert_true(canonical_basis(&pk, A) == EXIT_SUCCESS);
    // y-coordinate of P
    // t <- x²
    quadratic_field_square(&t, pk.P);
    // y <- Ax²
    quadratic_field_multiplication(&yP, A, t);
    // y <- Ax² + x
    quadratic_field_addition(&yP, yP, pk.P);
    // t <- x³
    quadratic_field_multiplication(&t, t, pk.P);
    // y <- x³ + Ax² + x
    quadratic_field_addition(&yP, yP, t);
    assert_true(quadratic_field_is_square(&yP, yP));

    // y-coordinate of Q
    y_coordinate_recovery_projective(&yQ, &zQ, pk.P, yP, pk.Q, pk.P_minus_Q, A);
    quadratic_field_inverse(&zQ, zQ);
    quadratic_field_multiplication(&yQ, yQ, zQ);
    assert_memory_not_equal(sizeof(quadratic_field_element_t), &yP, &yQ);

    // t <- x²
    quadratic_field_square(&t, pk.Q);
    // y <- Ax²
    quadratic_field_multiplication(&y, A, t);
    // y <- Ax² + x
    quadratic_field_addition(&y, y, pk.Q);
    // t <- x³
    quadratic_field_multiplication(&t, t, pk.Q);
    // y <- x³ + Ax² + x
    quadratic_field_addition(&y, y, t);
    quadratic_field_square(&t, yQ);
    assert_memory_equal(sizeof(quadratic_field_element_t), &y, &t);

    // y-coordinate of alice public key P
    quadratic_field_copy(&x, pk_alice.P);
    // t <- x²
    quadratic_field_square(&t, x);
    // y <- Ax²
    quadratic_field_multiplication(&y, A, t);
    // y <- Ax² + x
    quadratic_field_addition(&y, y, x);
    // t <- x³
    quadratic_field_multiplication(&t, t, x);
    // y <- x³ + Ax² + x
    quadratic_field_addition(&y, y, t);
    assert_true(quadratic_field_is_square(&y, y));

    to_twisted_edwards_model(&P_twisted, pk.P, yP);
    to_twisted_edwards_model(&Q_twisted, pk.Q, yQ);
    to_twisted_edwards_model(&K_twisted, x, y);

    // Twisted Edwards with a = A + 2 and d = A - 2
    quadratic_field_copy(&curve_edwards.a, A);
    prime_field_addition(curve_edwards.a.re, curve_edwards.a.re, MONTGOMERY_CONSTANT_ONE);
    prime_field_addition(curve_edwards.a.re, curve_edwards.a.re, MONTGOMERY_CONSTANT_ONE);
    quadratic_field_copy(&curve_edwards.d, A);
    prime_field_subtraction(curve_edwards.d.re, curve_edwards.d.re, MONTGOMERY_CONSTANT_ONE);
    prime_field_subtraction(curve_edwards.d.re, curve_edwards.d.re, MONTGOMERY_CONSTANT_ONE);

    // Testing c = 1
    two_dimensional_pohlig_hellman_bob(c,
                                       d,
                                       P_twisted,
                                       P_twisted,
                                       Q_twisted,
                                       curve_edwards);
    assert_memory_not_equal(sizeof(scalar_t), &c, &z);
    assert_memory_equal(sizeof(scalar_t), &d, &z);

    // Testing d = 1
    two_dimensional_pohlig_hellman_bob(c,
                                       d,
                                       Q_twisted,
                                       P_twisted,
                                       Q_twisted,
                                       curve_edwards);
    assert_memory_equal(sizeof(scalar_t), &c, &z);
    assert_memory_not_equal(sizeof(scalar_t), &d, &z);

    // +++++++ Generic case
    two_dimensional_pohlig_hellman_bob(c,
                                       d,
                                       K_twisted,
                                       P_twisted,
                                       Q_twisted,
                                       curve_edwards);
    assert_memory_not_equal(sizeof(scalar_t), &c, &z);
    assert_memory_not_equal(sizeof(scalar_t), &d, &z);

    two_dimensional_scalar_multiplication_bob(&K,
                                              pk.P,
                                              pk.Q,
                                              pk.P_minus_Q,
                                              c,
                                              d,
                                              A);

    x_only_point_copy(&R, K);
    assert_true(x_only_is_full_order_bob(&T, &R, A));

    quadratic_field_inverse(&t, K.Z);
    quadratic_field_multiplication(&t, t, K.X);
    assert_true(quadratic_field_is_equal(t, x));

    return MUNIT_OK;
}

static MunitResult test_random_private_key_sample(MUNIT_UNUSED const MunitParameter params[],
                                                  MUNIT_UNUSED void *user_data_or_fixture) {
    prime_field_element_t seed;
    RANDOM_PRIME_FIELD_ELEMENT(seed);
    uint8_t rnd[SECRET_KEY_BYTES_BOB + 1] = {0};
    uint8_t value_equality, expected_value = 0x1;
    uint32_t num_checks = 1000;
    random_bob_private_key(seed);

    scalar_t max = {0}, t;
    max[0] = 1;
    for (int i = 1; i < EXPONENT_BOB; i++) {
        memcpy(t, max, sizeof(uint64_t) * SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_shift_to_left(max, SUBGROUP_ORDER_64BITS_WORDS);
        multiprecision_addition(max, max, t, SUBGROUP_ORDER_64BITS_WORDS);
    }

    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    for (uint32_t j = 0; j < num_checks; j++) {
        random_private_key_sample(rnd, &shake_st);
        memcpy(t, rnd, SECRET_KEY_BYTES_BOB);
        value_equality = multiprecision_is_smaller(t, max, SUBGROUP_ORDER_64BITS_WORDS);
        assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);
        assert_true((0 <= (uint8_t) rnd[SECRET_KEY_BYTES_BOB]) && ((uint8_t) rnd[SECRET_KEY_BYTES_BOB] <= 3));
    }

    return MUNIT_OK;
}

static MunitResult test_sidh_pok(MUNIT_UNUSED const MunitParameter params[],
                                 MUNIT_UNUSED void *user_data_or_fixture) {
    uint8_t value_equality, expected_value = EXIT_SUCCESS;
    uint8_t commit[COMMITMENT_BYTES], inside[INSIGHT_BYTES], resp[RESPONSE_BYTES],
            pk_alice_bytes[PUBLIC_KEY_BYTES], sk_alice_bytes[SECRET_KEY_BYTES_ALICE];
    sidh_public_key_t pk_alice;
    sidh_private_key_t sk_alice;

    //Seed generation
    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    sidh_generate_private_key_alice(&sk_alice, &shake_st);
    memcpy(sk_alice_bytes, (const uint8_t *) sk_alice.key, SECRET_KEY_BYTES_ALICE);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
    sidh_public_key_to_bytes(pk_alice_bytes, &pk_alice);


    sidh_pok_commitment(commit, inside, sk_alice_bytes, &shake_st);

    value_equality = sidh_pok_response(resp, inside, 0);
    assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);

    value_equality = sidh_pok_verification(commit, resp, pk_alice_bytes, 0);
    assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);

    value_equality = sidh_pok_response(resp, inside, 1);
    assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);

    value_equality = sidh_pok_verification(commit, resp, pk_alice_bytes, 1);
    assert_memory_equal(sizeof(uint8_t), &expected_value, &value_equality);
    return EXIT_SUCCESS;
}


/*
 * Register test cases
 */

MunitTest test_pok[] = {
        TEST_CASE(test_elligator2),
        TEST_CASE(test_is_full_order_bob),
        TEST_CASE(test_canonical_basis_bob),
        TEST_CASE(test_canonical_basisx),
        TEST_CASE(test_two_dimensional_ladder),
        TEST_CASE(test_two_dimensional_pohlig_hellman),
        TEST_CASE(test_random_private_key_sample),
        TEST_CASE(test_sidh_pok),
        TEST_END
};

