#ifndef SIDH_POK_BENCHMARK_H
#define SIDH_POK_BENCHMARK_H

#include <stdint.h>
#include <time.h>
#include <fips202.h>
#include <parameters.h>
#include "munit.h"

#define KECCAK_SEED_LEN (size_t)(SECURITY_BITS / 8)
extern MunitParameterEnum benchmark_iterations[];
extern MunitParameterEnum benchmark_message_lengths[];

typedef struct timespec tspec_t;

typedef struct {
    uint8_t seed[KECCAK_SEED_LEN];
    keccak_state shake_st;
    long iterations;
    long message_num_bytes;
} benchmark_data_t;

void *benchmark_setup(const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture);

void benchmark_teardown(MUNIT_UNUSED void *user_data_or_fixture);

void get_cycles(uint64_t * result);

void get_clock(struct timespec *spec);

void report(const char *title, uint64_t *start_cycles, const uint64_t *stop_cycles, tspec_t *time_start,
            tspec_t *time_stop, size_t len);

long get_iterations(const MunitParameter params[]);

long get_message_num_bytes(const MunitParameter params[]);

#endif //SIDH_POK_BENCHMARK_H
