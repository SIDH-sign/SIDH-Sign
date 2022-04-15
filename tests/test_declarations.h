#ifndef SIDH_POK_TEST_DECLARATIONS_H
#define SIDH_POK_TEST_DECLARATIONS_H

#include "munit.h"
/**
 * Test cases
 */
extern MunitTest test_prime_fields[];
extern MunitTest test_quadratic_fields[];
extern MunitTest test_ecc[];
extern MunitTest test_isogeny[];
extern MunitTest test_sidh[];
extern MunitTest test_pok[];
extern MunitTest test_signature[];

/**
 * Benchmarks
 */
extern MunitTest benchmark_prime_fields[];
extern MunitTest benchmark_quadratic_fields[];
extern MunitTest benchmark_sidh[];
extern MunitTest benchmark_pok[];
extern MunitTest benchmark_signatures[];

#endif //SIDH_POK_TEST_DECLARATIONS_H
