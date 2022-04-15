#include "test_declarations.h"
#include "test_utils.h"
#include "benchmark.h"
#include <pok.h>


static void x_only_canonical_basis_bob_naive(x_only_point_t *P,
                                      x_only_point_t *Q,
                                      x_only_point_t *P_minus_Q,
                                      quadratic_field_element_t u_prime,
                                      quadratic_field_element_t A) {
    uint8_t flag;
    quadratic_field_element_t r_squared, r, u, t;
    x_only_point_t P3, Q3, P3_minus_Q3;

    // The QR r must lie on GF(p²) \ GF(p)
    quadratic_field_set_to_zero(&r);
    prime_field_set_to_one(r.im);
    quadratic_field_square(&u, u_prime);    // we break the rules by assuming u is a QR

    while (1) {
        // +++++++ P
        prime_field_addition(r.re, r.re, MONTGOMERY_CONSTANT_ONE);
        quadratic_field_square(&r_squared, r);
        flag = x_only_elligator2(P, u, r_squared, A);
        if (!flag) { continue; }
        quadratic_field_multiplication(&t, P->X, P->Z);
        flag = quadratic_field_is_square(&t, t);
        if (flag) { continue; }
        // +++++++ Q
        quadratic_field_multiplication(&Q->X, u, r_squared);
        quadratic_field_multiplication(&Q->X, Q->X, P->X);
        quadratic_field_copy(&Q->Z, P->Z);
        // +++++++ P - Q
        x_only_get_P_minus_Q(P_minus_Q, P->X, Q->X, P->Z, u_prime, r, A);
        // Full order-3ᵇ check
        if (!x_only_is_full_order_bob(&P3, P, A)) { continue; }
        if (!x_only_is_full_order_bob(&Q3, Q, A)) { continue; }
        if (!x_only_is_full_order_bob(&P3_minus_Q3, P_minus_Q, A)) { continue; }
        // Linear independency check by asking [3ᵇ⁻¹]P ≠ ±[3ᵇ⁻¹]Q
        if (x_only_point_is_equal(P3, Q3)) { continue; }
        break;
    }
}

static void canonical_basis_naive(sidh_public_key_t *output_basis, quadratic_field_element_t input_A) {
    quadratic_field_element_t u;
    x_only_point_t P, Q, P_minus_Q;

    // u = 1 + i
    prime_field_set_to_one(u.re);
    prime_field_set_to_one(u.im);

    x_only_canonical_basis_bob_naive(&P, &Q, &P_minus_Q, u, input_A);

    simultaneous_three_inverses(&P.Z, &Q.Z, &P_minus_Q.Z);
    quadratic_field_multiplication(&output_basis->P, P.X, P.Z);
    quadratic_field_multiplication(&output_basis->Q, Q.X, Q.Z);
    quadratic_field_multiplication(&output_basis->P_minus_Q, P_minus_Q.X, P_minus_Q.Z);
}

static MunitResult test_canonical_basis(MUNIT_UNUSED const MunitParameter params[], void *user_data_or_fixture) {
    benchmark_data_t *data = (benchmark_data_t *) user_data_or_fixture;
    unsigned i;
    uint64_t cycles_start[data->iterations];
    uint64_t cycles_stop[data->iterations];
    tspec_t time_start[data->iterations];
    tspec_t time_stop[data->iterations];

    quadratic_field_element_t A;
    sidh_private_key_t sk_alice;
    sidh_public_key_t pk_alice, pk = {0};

    for (i = 0; i < data->iterations; i++) {
        sidh_generate_private_key_alice(&sk_alice, &data->shake_st);
        sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
        get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);

        get_clock(&time_start[i]);
        get_cycles(&cycles_start[i]);
        canonical_basis(&pk, A);
        get_clock(&time_stop[i]);
        get_cycles(&cycles_stop[i]);
    }
    report("PoK Canonical Basis", cycles_start, cycles_stop, time_start,
           time_stop, data->iterations);
    return MUNIT_OK;
}

static MunitResult test_canonical_basis_naive(MUNIT_UNUSED const MunitParameter params[], void *user_data_or_fixture) {
    benchmark_data_t *data = (benchmark_data_t *) user_data_or_fixture;
    unsigned i;
    uint64_t cycles_start[data->iterations];
    uint64_t cycles_stop[data->iterations];
    tspec_t time_start[data->iterations];
    tspec_t time_stop[data->iterations];

    quadratic_field_element_t A;
    sidh_private_key_t sk_alice;
    sidh_public_key_t pk_alice, pk = {0};

    for (i = 0; i < data->iterations; i++) {
        sidh_generate_private_key_alice(&sk_alice, &data->shake_st);
        sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
        get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);

        get_clock(&time_start[i]);
        get_cycles(&cycles_start[i]);
        canonical_basis_naive(&pk, A);
        get_clock(&time_stop[i]);
        get_cycles(&cycles_stop[i]);
    }
    report("PoK Canonical Basis Naive", cycles_start, cycles_stop, time_start,
           time_stop, data->iterations);
    return MUNIT_OK;
}

static MunitResult test_two_dimensional_pohlig_hellman(MUNIT_UNUSED const MunitParameter params[],
                                                       void *user_data_or_fixture) {
    benchmark_data_t *data = (benchmark_data_t *) user_data_or_fixture;
    unsigned i;
    uint64_t cycles_start[data->iterations];
    uint64_t cycles_stop[data->iterations];
    tspec_t time_start[data->iterations];
    tspec_t time_stop[data->iterations];

    scalar_t c, d, k;
    quadratic_field_element_t A, yP, yQ, zQ, t, y, x, a;
    sidh_private_key_t sk_alice;
    sidh_public_key_t pk_alice, pk = {0};
    twisted_edwards_curve_t curve_edwards;
    projective_point_t P_twisted, Q_twisted, K_twisted;

    x_only_point_t R;

    // setup
    sidh_generate_private_key_alice(&sk_alice, &data->shake_st);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);
    get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);
    canonical_basis(&pk, A);
    // y-coordinate of P
    quadratic_field_square(&t, pk.P);
    quadratic_field_multiplication(&yP, A, t);
    quadratic_field_addition(&yP, yP, pk.P);
    quadratic_field_multiplication(&t, t, pk.P);
    quadratic_field_addition(&yP, yP, t);

    // y-coordinate of Q
    y_coordinate_recovery_projective(&yQ, &zQ, pk.P, yP, pk.Q, pk.P_minus_Q, A);
    quadratic_field_inverse(&zQ, zQ);
    quadratic_field_multiplication(&yQ, yQ, zQ);

    // edward curve
    quadratic_field_copy(&curve_edwards.a, A);
    prime_field_addition(curve_edwards.a.re, curve_edwards.a.re, MONTGOMERY_CONSTANT_ONE);
    prime_field_addition(curve_edwards.a.re, curve_edwards.a.re, MONTGOMERY_CONSTANT_ONE);
    quadratic_field_copy(&curve_edwards.d, A);
    prime_field_subtraction(curve_edwards.d.re, curve_edwards.d.re, MONTGOMERY_CONSTANT_ONE);
    prime_field_subtraction(curve_edwards.d.re, curve_edwards.d.re, MONTGOMERY_CONSTANT_ONE);

    to_twisted_edwards_model(&P_twisted, pk.P, yP);
    to_twisted_edwards_model(&Q_twisted, pk.Q, yQ);

    get_curve_from_points(&a, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);

    for (i = 0; i < data->iterations; i++) {
        random_bob_private_key(k);

        x_only_three_point_ladder(&R,
                                  pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q,
                                  k, SUBGROUP_ORDER_BITS_ALICE,
                                  a);

        quadratic_field_inverse(&R.Z, R.Z);
        quadratic_field_multiplication(&R.X, R.Z, R.X);
        quadratic_field_set_to_one(&R.Z);

        // y-coordinate of alice public key P
        quadratic_field_copy(&x, R.X);
        quadratic_field_square(&t, x);
        quadratic_field_multiplication(&y, A, t);
        quadratic_field_addition(&y, y, x);
        quadratic_field_multiplication(&t, t, x);
        quadratic_field_addition(&y, y, t);

        to_twisted_edwards_model(&K_twisted, x, y);

        // Benchmark
        get_clock(&time_start[i]);
        get_cycles(&cycles_start[i]);
        two_dimensional_pohlig_hellman_bob(c, d, K_twisted, P_twisted, Q_twisted, curve_edwards);
        get_clock(&time_stop[i]);
        get_cycles(&cycles_stop[i]);
    }
    report("PoK Two Dimensional pohlig hellman", cycles_start, cycles_stop, time_start,
           time_stop, data->iterations);
    return MUNIT_OK;
}

static MunitResult test_two_dimensional_ladder(MUNIT_UNUSED const MunitParameter params[], void *user_data_or_fixture) {

    benchmark_data_t *data = (benchmark_data_t *) user_data_or_fixture;
    unsigned i;
    uint64_t cycles_start[data->iterations];
    uint64_t cycles_stop[data->iterations];
    tspec_t time_start[data->iterations];
    tspec_t time_stop[data->iterations];

    quadratic_field_element_t A;
    sidh_private_key_t sk_alice, sk_bob;
    sidh_public_key_t pk_alice, pk = {0};
    x_only_point_t T;
    scalar_t z;

    sidh_generate_private_key_alice(&sk_alice, &data->shake_st);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);

    get_curve_from_points(&A, pk_alice.P, pk_alice.Q, pk_alice.P_minus_Q);

    for (i = 0; i < data->iterations; i++) {
        random_bob_private_key(sk_bob.key);
        random_bob_private_key(z);


        get_clock(&time_start[i]);
        get_cycles(&cycles_start[i]);
        two_dimensional_scalar_multiplication_bob(&T, pk.P, pk.Q, pk.P_minus_Q, z, sk_bob.key, A);
        get_clock(&time_stop[i]);
        get_cycles(&cycles_stop[i]);

    }
    report("PoK Two Dimensional Ladder", cycles_start, cycles_stop, time_start,
           time_stop, data->iterations);
    return MUNIT_OK;
}

/*
 * Register benchmark cases
 */

// NOLINTNEXTLINE
MunitTest benchmark_pok[] = {
        BENCHMARK(test_canonical_basis, benchmark_setup, benchmark_teardown, benchmark_iterations),
        BENCHMARK(test_canonical_basis_naive, benchmark_setup, benchmark_teardown, benchmark_iterations),
        BENCHMARK(test_two_dimensional_pohlig_hellman, benchmark_setup, benchmark_teardown, benchmark_iterations),
        BENCHMARK(test_two_dimensional_ladder, benchmark_setup, benchmark_teardown, benchmark_iterations),
        TEST_END
};

