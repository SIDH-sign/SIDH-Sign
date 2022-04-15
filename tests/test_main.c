#include "test_declarations.h"
#include "test_utils.h"


// NOLINTNEXTLINE
static MunitSuite benchmark_suites[] = {
        SUITE_IT("prime-fields/", benchmark_prime_fields, 10000),
        SUITE_IT("quadratic-fields/", benchmark_quadratic_fields, 10000),
        SUITE("sidh/", benchmark_sidh),
        SUITE("pok/", benchmark_pok),
        SUITE("signatures/", benchmark_signatures),
        SUITE_END
};

// NOLINTNEXTLINE
static MunitSuite test_suites[] = {
        SUITE_IT("prime-fields/", test_prime_fields, 100),
        SUITE_IT("quadratic-fields/", test_quadratic_fields, 100),
        SUITE("ecc/", test_ecc),
        SUITE("isogeny/", test_isogeny),
        SUITE("sidh/", test_sidh),
        SUITE("pok/", test_pok),
        SUITE("signatures/", test_signature),
        SUITE_END
};

static MunitSuite chained_suites[] = {
        SUITE_OF_SUITE("tests/", test_suites),
        SUITE_OF_SUITE("benchmarks/", benchmark_suites),
        SUITE_END
};

static const MunitSuite test_suite = {"", NULL, chained_suites, 0, MUNIT_SUITE_OPTION_NONE};

/*
 * Main function
 */
int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)]) {

    return munit_suite_main(&test_suite, NULL, argc, argv);
}
