#include "test_declarations.h"
#include "test_utils.h"
#include "benchmark.h"
#include <sidh.h>

static MunitResult key_gen_and_agreement(MUNIT_UNUSED const MunitParameter params[], void *user_data_or_fixture) {

    benchmark_data_t *data = (benchmark_data_t *) user_data_or_fixture;
    unsigned i;
    uint64_t cycles_gen_alice_start[data->iterations];
    uint64_t cycles_gen_alice_stop[data->iterations];
    uint64_t cycles_gen_bob_start[data->iterations];
    uint64_t cycles_gen_bob_stop[data->iterations];
    uint64_t cycles_agree_alice_start[data->iterations];
    uint64_t cycles_agree_alice_stop[data->iterations];
    uint64_t cycles_agree_bob_start[data->iterations];
    uint64_t cycles_agree_bob_stop[data->iterations];

    tspec_t time_gen_alice_start[data->iterations];
    tspec_t time_gen_alice_stop[data->iterations];
    tspec_t time_gen_bob_start[data->iterations];
    tspec_t time_gen_bob_stop[data->iterations];
    tspec_t time_agree_alice_start[data->iterations];
    tspec_t time_agree_alice_stop[data->iterations];
    tspec_t time_agree_bob_start[data->iterations];
    tspec_t time_agree_bob_stop[data->iterations];

    sidh_private_key_t sk_alice, sk_bob;
    sidh_public_key_t pk_alice, pk_bob;
    sidh_shared_secret_t ss_alice, ss_bob;

    for (i = 0; i < data->iterations; i++) {
        //Gen Alice key pair
        get_clock(&time_gen_alice_start[i]);
        get_cycles(&cycles_gen_alice_start[i]);
        sidh_generate_private_key_alice(&sk_alice, &data->shake_st);
        sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
        get_clock(&time_gen_alice_stop[i]);
        get_cycles(&cycles_gen_alice_stop[i]);

        //Gen Bob key pair
        get_clock(&time_gen_bob_start[i]);
        get_cycles(&cycles_gen_bob_start[i]);
        sidh_generate_private_key_bob(&sk_bob, &data->shake_st);
        sidh_get_public_key_from_private_key_bob(&pk_bob, &sk_bob);
        get_clock(&time_gen_bob_stop[i]);
        get_cycles(&cycles_gen_bob_stop[i]);

        // secret sharing derivation ALICE
        get_clock(&time_agree_alice_start[i]);
        get_cycles(&cycles_agree_alice_start[i]);
        sidh_key_agreement_alice(&ss_alice, &sk_alice, &pk_bob);
        get_clock(&time_agree_alice_stop[i]);
        get_cycles(&cycles_agree_alice_stop[i]);


        // secret sharing derivation BOB
        get_clock(&time_agree_bob_start[i]);
        get_cycles(&cycles_agree_bob_start[i]);
        sidh_key_agreement_bob(&ss_bob, &sk_bob, &pk_alice);
        get_clock(&time_agree_bob_stop[i]);
        get_cycles(&cycles_agree_bob_stop[i]);

        assert_memory_equal(sizeof(quadratic_field_element_t), &ss_alice.curve, &ss_bob.curve);
    }
    report("SIDH key gen Alice", cycles_gen_alice_start, cycles_gen_alice_stop, time_gen_alice_start,
           time_gen_alice_stop, data->iterations);
    report("SIDH key gen Bob", cycles_gen_bob_start, cycles_gen_bob_stop, time_gen_bob_start,
           time_gen_bob_stop, data->iterations);

    report("SIDH key key agreement Alice", cycles_agree_alice_start, cycles_agree_alice_stop, time_agree_alice_start,
           time_agree_alice_stop, data->iterations);
    report("SIDH key key agreement Bob", cycles_agree_bob_start, cycles_agree_bob_stop, time_agree_bob_start,
           time_agree_bob_stop, data->iterations);
    return MUNIT_OK;
}

/*
 * Register benchmark cases
 */

// NOLINTNEXTLINE
MunitTest benchmark_sidh[] = {
        BENCHMARK(key_gen_and_agreement, benchmark_setup, benchmark_teardown, benchmark_iterations),
        TEST_END
};

