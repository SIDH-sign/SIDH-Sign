/**
 * Cycle count, median and average based on XMSS Universal Public Domain
 * reference code available at 'https://github.com/XMSS/xmss-reference'
 *
 * Timespec functions based on Universal Public Domain code
 * available at 'https://github.com/solemnwarning/timespec'
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "benchmark.h"
#include "test_utils.h"

#define NSEC_PER_SEC 1000000000
#define BENCHMARK_LOG_LEVEL MUNIT_LOG_INFO
#define BENCHMARK_LOG_FILE NULL
#define BENCHMARK_LEFT_PAD ""
#define BENCHMARK_DEFAULT_ITERATIONS 10
#define BENCHMARK_DEFAULT_MESSAGE_LEN 64
#define ITERATIONS_PARAM "iterations"
#define MESSAGE_LEN_PARAM "mlen"

static char *iterations[] = {"10", NULL};
static char *message_lengths[] = {"64", NULL};

MunitParameterEnum benchmark_iterations[] = {
        {ITERATIONS_PARAM, iterations},
        PARAM_END
};

MunitParameterEnum benchmark_message_lengths[] = {
        {ITERATIONS_PARAM, iterations},
        {MESSAGE_LEN_PARAM, message_lengths},
        PARAM_END
};

static int cmp_uint64(const void *a, const void *b) {
    if (*(uint64_t *) a < *(uint64_t *) b) {
        return -1;
    }
    if (*(uint64_t *) a > *(uint64_t *) b) {
        return 1;
    }
    return 0;
}


static uint64_t median(uint64_t *list, const size_t len) {
    //Call after sorting!
    if (len < 2) {
        return 0;
    }

    if ((len % 2) == 1) { //odd len, get middle
        return list[len / 2];
    } else {
        return (list[len / 2 - 1] + list[len / 2]) / 2;
    }
}

static uint64_t first_quartile(uint64_t *list, const size_t len) {
    //Call after sorting!
    if (len < 4) {
        return 0;
    }
    size_t half = (int) (len / 2);
    return median(list, half);
}


static uint64_t third_quartile(uint64_t *list, const size_t len) {
    //Call after sorting!
    if (len < 4) {
        return 0;
    }

    size_t half = (int) (len / 2);
    if ((len % 2) == 1) {
        return median(list + half + 1, half);
    } else {
        return median(list + half, half);
    }
}


static double timespec_to_double(const tspec_t *ts) {
    return ((double) (ts->tv_sec) + ((double) (ts->tv_nsec) / NSEC_PER_SEC));
}


static uint64_t average(const uint64_t *list, const size_t len) {
    uint64_t acc = 0;
    size_t i;
    for (i = 0; i < len; i++) {
        acc += list[i];
    }
    return acc / (len);
}

static void timespec_normalise(tspec_t *ts) {
    while (ts->tv_nsec >= NSEC_PER_SEC) {
        ++(ts->tv_sec);
        ts->tv_nsec -= NSEC_PER_SEC;
    }

    while (ts->tv_nsec <= -NSEC_PER_SEC) {
        --(ts->tv_sec);
        ts->tv_nsec += NSEC_PER_SEC;
    }

    if (ts->tv_nsec < 0) {
        /* Negative nanoseconds isn't valid according to POSIX.
         * Decrement tv_sec and roll tv_nsec over.
        */

        --(ts->tv_sec);
        ts->tv_nsec = (NSEC_PER_SEC + ts->tv_nsec);
    }
}

static tspec_t timespec_add(tspec_t *ts1, tspec_t *ts2) {
    tspec_t result = {.tv_sec = 0, .tv_nsec = 0};
    timespec_normalise(ts1);
    timespec_normalise(ts2);

    result.tv_sec = ts1->tv_sec + ts2->tv_sec;
    result.tv_nsec = ts1->tv_nsec + ts2->tv_nsec;
    timespec_normalise(&result);

    return result;
}

static tspec_t timespec_sub(tspec_t *ts1, tspec_t *ts2) {
    tspec_t result = {.tv_sec = 0, .tv_nsec = 0};
    timespec_normalise(ts1);
    timespec_normalise(ts2);

    result.tv_sec = ts1->tv_sec - ts2->tv_sec;
    result.tv_nsec = ts1->tv_nsec - ts2->tv_nsec;
    timespec_normalise(&result);

    return result;
}

static double median_time(tspec_t *list, const size_t len) {
    //Call after sorting!
    if (len < 2) {
        return 0;
    }

    if ((len % 2) == 1) { //odd len, get middle
        return timespec_to_double(&list[len / 2]);
    } else {
        tspec_t aux = timespec_add(&list[len / 2 - 1], &list[len / 2]);
        return timespec_to_double(&aux) / 2;
    }
}

static double first_quartile_time(tspec_t *list, const size_t len) {
    //Call after sorting!
    if (len < 4) {
        return 0;
    }
    size_t half = (int) (len / 2);
    return median_time(list, half);
}


static double third_quartile_time(tspec_t *list, const size_t len) {
    //Call after sorting!
    if (len < 4) {
        return 0;
    }

    size_t half = (int) (len / 2);
    if ((len % 2) == 1) {
        return median_time(list + half + 1, half);
    } else {
        return median_time(list + half, half);
    }
}


/** \fn int cmp_timespec(struct timespec ts1, struct timespec ts2)
 *  \brief Returns (1, 0, -1) if ts1 is (greater than, equal to, less than) to ts2.
*/
int cmp_timespec(const void *a, const void *b)
{
    tspec_t *ts1 = (tspec_t *) a;
    tspec_t *ts2 = (tspec_t *) b;
    timespec_normalise(ts1);
    timespec_normalise(ts2);

    if(ts1->tv_sec == ts2->tv_sec && ts1->tv_nsec == ts2->tv_nsec)
    {
        return 0;
    }
    else if((ts1->tv_sec > ts2->tv_sec)
            || (ts1->tv_sec == ts2->tv_sec && ts1->tv_nsec > ts2->tv_nsec))
    {
        return 1;
    }
    else {
        return -1;
    }
}

static void print_report(const char *title, uint64_t *cycles, const size_t len, const tspec_t *running_time, tspec_t *times) {
#if defined(_P377_)
    char *prime = "p377";
#elif defined(_P434_)
    char * prime = "p434";
#elif defined(_P503_)
    char * prime = "p503";
#elif defined(_P546_)
    char * prime = "p546";
#elif defined(_P610_)
    char * prime = "p610";
#elif defined(_P697_)
    char * prime = "p697";
#elif defined(_P751_)
    char * prime = "p751";
#else
    char * prime = "p434";
#endif
    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %s %s with %zu iterations", BENCHMARK_LEFT_PAD,
                  title, prime,
                  len);
    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %lf \t\t seconds (total)", BENCHMARK_LEFT_PAD,
                  timespec_to_double(running_time));

    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %lf\t\t seconds [Q1]", BENCHMARK_LEFT_PAD,
                  first_quartile_time(times, len));
    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %lf\t\t seconds [median]", BENCHMARK_LEFT_PAD,
                  median_time(times, len));
    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %lf\t\t seconds [Q3]", BENCHMARK_LEFT_PAD,
                  third_quartile_time(times, len));
    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %lf \t\t seconds [average]",
                  BENCHMARK_LEFT_PAD, timespec_to_double(running_time) / (double) len);

    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %" PRIu64 "\t\t cycles [Q1]", BENCHMARK_LEFT_PAD,
                  first_quartile(cycles, len));
    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %" PRIu64 "\t\t cycles [median]", BENCHMARK_LEFT_PAD,
                  median(cycles, len));
    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %" PRIu64 "\t\t cycles [Q3]", BENCHMARK_LEFT_PAD,
                  third_quartile(cycles, len));
    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, "%-30s %" PRIu64 "\t\t cycles [average]", BENCHMARK_LEFT_PAD,
                  average(cycles, len - 1));
    munit_logf_ex(BENCHMARK_LOG_LEVEL, BENCHMARK_LOG_FILE, 0, " ");
}


void *benchmark_setup(const MunitParameter params[], MUNIT_UNUSED void *user_data_or_fixture) {

    benchmark_data_t *data = (benchmark_data_t *) munit_malloc(sizeof(benchmark_data_t));

    data->iterations = get_iterations(params);
    data->message_num_bytes = get_message_num_bytes(params);

    munit_rand_memory(KECCAK_SEED_LEN, (uint8_t *) data->seed);

    shake256_init(&data->shake_st);
    shake256_absorb(&data->shake_st, data->seed, KECCAK_SEED_LEN);
    shake256_finalize(&data->shake_st);

    return (void *) data;
}

void benchmark_teardown(MUNIT_UNUSED void *user_data_or_fixture) {
    benchmark_data_t *data = (benchmark_data_t *) user_data_or_fixture;
    free(data);
}

// NOLINTNEXTLINE
void get_cycles(uint64_t *result) {
    __asm volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
    : "=a" (*result)::"%rdx");
}

void get_clock(tspec_t *spec) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, spec);
}


void report(const char *title, uint64_t *start_cycles, const uint64_t *stop_cycles, tspec_t *time_start,
            tspec_t *time_stop, size_t len) {
    size_t i;
    tspec_t running_time = {.tv_sec = 0, .tv_nsec = 0};
    tspec_t aux;
    for (i = 0; i < len; i++) {
        start_cycles[i] = stop_cycles[i] - start_cycles[i];
        aux = timespec_sub(&time_stop[i], &time_start[i]);
        time_start[i].tv_nsec = aux.tv_nsec;
        time_start[i].tv_sec = aux.tv_sec;
        running_time = timespec_add(&running_time, &aux);
//        time_start[i].tv_sec = running_time.tv_sec;
    }
    qsort(start_cycles, len, sizeof(uint64_t), cmp_uint64);
    qsort(time_start, len, sizeof(tspec_t), cmp_timespec);
    print_report(title, start_cycles, len, &running_time, time_start);
}

long get_iterations(const MunitParameter params[]) {
    const char *it = munit_parameters_get(params, ITERATIONS_PARAM);
    if (it == NULL) {
        return BENCHMARK_DEFAULT_ITERATIONS;
    }
    long ret = strtol(it, NULL, 10);
    if (ret == 0) {
        return BENCHMARK_DEFAULT_ITERATIONS;
    }
    return ret;
}

long get_message_num_bytes(const MunitParameter params[]) {
    const char *it = munit_parameters_get(params, MESSAGE_LEN_PARAM);
    if (it == NULL) {
        return BENCHMARK_DEFAULT_MESSAGE_LEN;
    }
    long ret = strtol(it, NULL, 10);
    if (ret == 0) {
        return BENCHMARK_DEFAULT_MESSAGE_LEN;
    }
    return ret;
}
