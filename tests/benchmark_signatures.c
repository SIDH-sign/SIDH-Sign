#include "test_declarations.h"
#include "test_utils.h"
#include "benchmark.h"
#include <signature.h>

static MunitResult sign_verify_message(MUNIT_UNUSED const MunitParameter params[], void *user_data_or_fixture) {
    benchmark_data_t *data = (benchmark_data_t *) user_data_or_fixture;
    unsigned i, check = 0;
    uint64_t cycles_gen_start[data->iterations];
    uint64_t cycles_gen_stop[data->iterations];
    uint64_t cycles_sign_start[data->iterations];
    uint64_t cycles_sign_stop[data->iterations];
    uint64_t cycles_ver_start[data->iterations];
    uint64_t cycles_ver_stop[data->iterations];
    struct timespec time_gen_start[data->iterations];
    struct timespec time_gen_stop[data->iterations];
    struct timespec time_sign_start[data->iterations];
    struct timespec time_sign_stop[data->iterations];
    struct timespec time_ver_start[data->iterations];
    struct timespec time_ver_stop[data->iterations];

    uint8_t private_key[SECRET_KEY_BYTES];
    uint8_t public_key[PUBLIC_KEY_BYTES];
    uint8_t signature[SIGNATURE_BYTES] = {0};
    uint8_t message[data->message_num_bytes];

    for (i = 0; i < data->iterations; i++) {
        get_clock(&time_gen_start[i]);
        get_cycles(&cycles_gen_start[i]);
        check |= sidh_signature_key_generation(private_key, public_key, &data->shake_st);
        get_cycles(&cycles_gen_stop[i]);
        get_clock(&time_gen_stop[i]);

        data->seed[0]++;
        shake256(message, data->message_num_bytes, data->seed, KECCAK_SEED_LEN);

        get_clock(&time_sign_start[i]);
        get_cycles(&cycles_sign_start[i]);
        sidh_signature_sign(signature, message, data->message_num_bytes, private_key, public_key, &data->shake_st);
        get_cycles(&cycles_sign_stop[i]);
        get_clock(&time_sign_stop[i]);

        get_clock(&time_ver_start[i]);
        get_cycles(&cycles_ver_start[i]);
        check |= sidh_signature_verify(signature, message, data->message_num_bytes, public_key);
        get_cycles(&cycles_ver_stop[i]);
        get_clock(&time_ver_stop[i]);
    }
    assert(check == EXIT_SUCCESS);
    report("SI-sign KeyGen", cycles_gen_start, cycles_gen_stop, time_gen_start, time_gen_stop,
           data->iterations);
    report("SI-sign Sign", cycles_sign_start, cycles_sign_stop, time_sign_start, time_sign_stop,
           data->iterations);
    report("SI-sign Verify", cycles_ver_start, cycles_ver_stop, time_ver_start, time_ver_stop,
           data->iterations);
    return MUNIT_OK;
}

/*
 * Register benchmark cases
 */

// NOLINTNEXTLINE
MunitTest benchmark_signatures[] = {
//        BENCHMARK(key_generation, signature_setup, signature_teardown, benchmark_iterations),
        BENCHMARK(sign_verify_message, benchmark_setup, benchmark_teardown, benchmark_message_lengths),
        TEST_END
};
