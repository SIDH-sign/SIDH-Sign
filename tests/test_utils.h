#ifndef SIDH_POK_TEST_UTILS_H
#define SIDH_POK_TEST_UTILS_H

#include "munit.h"
#include <elliptic_curve.h>
#include <utilities.h>

static inline __attribute__((always_inline)) void escape(void *p) { __asm__ __volatile__("" : "+m,r"(p) : : "memory"); }

#define TEST_END \
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
#define SUITE_END \
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
#define PARAM_END \
    { NULL, NULL }


#define TEST_CASE(test_func) \
    { #test_func, test_func, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }

#define TEST_CASE_SETUP(test_func, setup, teardown) \
    { #test_func, test_func, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL }

#define BENCHMARK(test_func, setup, teardown, params) \
    { #test_func, test_func, setup, teardown, MUNIT_TEST_OPTION_SINGLE_ITERATION, params }

#define SUITE(suite_name, test_cases) \
    { suite_name, test_cases, NULL, 0, MUNIT_SUITE_OPTION_NONE }

#define SUITE_IT(suite_name, test_cases, iterations) \
    { suite_name, test_cases, NULL, iterations, MUNIT_SUITE_OPTION_NONE }

#define SUITE_OF_SUITE(suite_name, nested_suite) \
    { suite_name, NULL, nested_suite, 0, MUNIT_SUITE_OPTION_NONE }


#define RANDOM_PRIME_FIELD_ELEMENT(out) \
{                                                                                           \
    munit_rand_memory(FIELD_BYTES, (munit_uint8_t *) (out));                                \
    (out)[FIELD_64BITS_WORDS - 1] &= MASK_FIELD_ELEMENT;                                    \
    while (!multiprecision_is_smaller((out), PRIME_CHARACTERISTIC, FIELD_64BITS_WORDS)) {  \
        munit_rand_memory(FIELD_BYTES, (munit_uint8_t *) (out));                            \
        (out)[FIELD_64BITS_WORDS - 1] &= MASK_FIELD_ELEMENT;                                \
    }                                                                                       \
}

#define RANDOM_SEED(out) \
{                                                                                   \
    munit_rand_memory(SECURITY_BITS / 8, (munit_uint8_t *) (out));            \
}

static void random_bob_private_key(uint64_t output[SUBGROUP_ORDER_64BITS_WORDS]) {
    munit_rand_memory(SECRET_KEY_BYTES_BOB, (munit_uint8_t *) output);
    output[SUBGROUP_ORDER_64BITS_WORDS - 1] &= MASK_BOB_64BITS;
}

static void initialize_generators_alice(quadratic_field_element_t *P, quadratic_field_element_t *Q,
                                 quadratic_field_element_t *P_minus_Q) {
    prime_field_copy(P->re, PUBLIC_POINT_GENERATORS_ALICE);
    prime_field_copy(P->im, &PUBLIC_POINT_GENERATORS_ALICE[FIELD_64BITS_WORDS]);
    prime_field_copy(Q->re, &PUBLIC_POINT_GENERATORS_ALICE[2 * FIELD_64BITS_WORDS]);
    prime_field_copy(Q->im, &PUBLIC_POINT_GENERATORS_ALICE[3 * FIELD_64BITS_WORDS]);
    prime_field_copy(P_minus_Q->re, &PUBLIC_POINT_GENERATORS_ALICE[4 * FIELD_64BITS_WORDS]);
    prime_field_copy(P_minus_Q->im, &PUBLIC_POINT_GENERATORS_ALICE[5 * FIELD_64BITS_WORDS]);
}

static void initialize_generators_bob(quadratic_field_element_t *P, quadratic_field_element_t *Q,
                               quadratic_field_element_t *P_minus_Q) {
    prime_field_copy(P->re, PUBLIC_POINT_GENERATORS_BOB);
    prime_field_copy(P->im, &PUBLIC_POINT_GENERATORS_BOB[FIELD_64BITS_WORDS]);
    prime_field_copy(Q->re, &PUBLIC_POINT_GENERATORS_BOB[2 * FIELD_64BITS_WORDS]);
    prime_field_copy(Q->im, &PUBLIC_POINT_GENERATORS_BOB[3 * FIELD_64BITS_WORDS]);
    prime_field_copy(P_minus_Q->re, &PUBLIC_POINT_GENERATORS_BOB[4 * FIELD_64BITS_WORDS]);
    prime_field_copy(P_minus_Q->im, &PUBLIC_POINT_GENERATORS_BOB[5 * FIELD_64BITS_WORDS]);
}

static void quadratic_field_random(quadratic_field_element_t *output) {
    RANDOM_PRIME_FIELD_ELEMENT(output->re);
    RANDOM_PRIME_FIELD_ELEMENT(output->im);
}

#endif //SIDH_POK_TEST_UTILS_H
