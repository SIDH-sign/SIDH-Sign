#include "test_declarations.h"
#include "test_utils.h"
#include "utilities.h"
#include <signature.h>
#include <printf.h>

static MunitResult key_generation(MUNIT_UNUSED const MunitParameter params[],
                                  MUNIT_UNUSED void *user_data_or_fixture) {
    uint8_t private_key[SECRET_KEY_BYTES] = {0};
    uint8_t public_key[PUBLIC_KEY_BYTES] = {0};

    uint8_t private_key2[SECRET_KEY_BYTES] = {0};
    uint8_t public_key2[PUBLIC_KEY_BYTES] = {0};

    //Seed generation
    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    keccak_state shake_st_copy;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    memcpy(&shake_st_copy, &shake_st, sizeof(shake_st));

    assert_uint8(EXIT_SUCCESS, ==, sidh_signature_key_generation(private_key, public_key, &shake_st));

    assert_uint8(EXIT_SUCCESS, ==, sidh_signature_key_generation(private_key2, public_key2, &shake_st_copy));

    assert_memory_equal(PUBLIC_KEY_BYTES, public_key, public_key2);
    assert_memory_equal(sizeof(private_key), private_key, private_key2);

    memset(public_key2, 0, PUBLIC_KEY_BYTES);
    memset(private_key2, 0, sizeof(private_key2));

    assert_uint8(EXIT_SUCCESS, ==, sidh_signature_key_generation(private_key2, public_key2, &shake_st));
    assert_memory_not_equal(PUBLIC_KEY_BYTES, public_key, public_key2);
    assert_memory_not_equal(sizeof(private_key), private_key, private_key2);

    return MUNIT_OK;
}


static MunitResult sidh_signature(MUNIT_UNUSED const MunitParameter params[],
                                  MUNIT_UNUSED void *user_data_or_fixture) {
    uint64_t message_length = 10000;
    uint8_t private_key[SECRET_KEY_BYTES] = {0};
    uint8_t public_key[PUBLIC_KEY_BYTES] = {0};
    uint8_t signature[SIGNATURE_BYTES] = {0};
    uint8_t message[message_length];

    memset(message, 0, message_length);

    uint8_t seed[SECURITY_BITS / 8] = {0};
    RANDOM_SEED(seed)
    keccak_state shake_st;
    shake256_init(&shake_st);
    shake256_absorb(&shake_st, (uint8_t *) seed, sizeof(seed));
    shake256_finalize(&shake_st);

    assert_uint8(EXIT_SUCCESS, ==, sidh_signature_key_generation(private_key, public_key, &shake_st));

    seed[0]++;
    shake256(message, sizeof(message), seed, sizeof(seed));

    assert_uint8(EXIT_SUCCESS, ==,
                       sidh_signature_sign(signature, message, message_length, private_key, public_key,
                                           &shake_st));
    assert_uint8(EXIT_SUCCESS, ==, sidh_signature_verify(signature, message, message_length, public_key));
    return MUNIT_OK;
}

MunitTest test_signature[] = {
        TEST_CASE(key_generation),
        TEST_CASE(sidh_signature),
        TEST_END
};
