#include "test_declarations.h"
#include "test_utils.h"
#include <sidh.h>

/*
 * Test cases
 */

static MunitResult sidh(MUNIT_UNUSED const MunitParameter params[],
                      MUNIT_UNUSED void *user_data_or_fixture) {
    sidh_private_key_t sk_alice, sk_bob;
    sidh_public_key_t pk_alice, pk_bob;
    sidh_shared_secret_t ss_alice, ss_bob;

    //Seed generation
    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);


    // Public key generation
    sidh_generate_private_key_alice(&sk_alice, &shake_st);
    sidh_get_public_key_from_private_key_alice(&pk_alice, &sk_alice);

    sidh_generate_private_key_bob(&sk_bob, &shake_st);
    sidh_get_public_key_from_private_key_bob(&pk_bob, &sk_bob);

    // secret sharing derivation
    sidh_key_agreement_alice(&ss_alice, &sk_alice, &pk_bob);
    sidh_key_agreement_bob(&ss_bob, &sk_bob, &pk_alice);

    assert_memory_equal(sizeof(quadratic_field_element_t), &ss_alice.curve, &ss_bob.curve);

    return MUNIT_OK;
}

/*
 * Register test cases
 */

MunitTest test_sidh[] = {
        TEST_CASE(sidh),
        TEST_END
};
