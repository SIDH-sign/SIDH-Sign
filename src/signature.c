//
// Digital signature scheme based on PoK
//

#include <stdlib.h>
#include <string.h>
#include "signature.h"
#include "sidh.h"

uint8_t sidh_signature_key_generation(uint8_t private_key_bytes[SECRET_KEY_BYTES],
                                      uint8_t public_key_bytes[PUBLIC_KEY_BYTES],
                                      keccak_state *state) {
    sidh_private_key_t private_key;
    sidh_public_key_t public_key;

    sidh_generate_private_key_alice(&private_key, state);
    sidh_get_public_key_from_private_key_alice(&public_key, &private_key);

    sidh_public_key_to_bytes(public_key_bytes, &public_key);
    sidh_private_key_to_bytes_alice(private_key_bytes, &private_key);
    return EXIT_SUCCESS;
}

uint8_t sidh_signature_sign(uint8_t signature[SIGNATURE_BYTES],
                            const uint8_t *message,
                            uint64_t message_length,
                            const uint8_t private_key[SECRET_KEY_BYTES],
                            const uint8_t public_key[PUBLIC_KEY_BYTES],
                            keccak_state *state) {
    if (message_length == 0) {
        return EXIT_FAILURE;
    }
    uint8_t returned_value = 0;
    uint8_t insights[INSIGHT_BYTES * SECURITY_BITS] = {0};
    uint8_t challenge_bytes[SECURITY_BITS / 8] = {0};
    uint8_t statement[PUBLIC_KEY_BYTES + (SECURITY_BITS / 8)] = {0};

    for (uint8_t i = 0; i < SECURITY_BITS / 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            sidh_pok_commitment(&signature[COMMITMENT_BYTES * ((i * 8) + j)],
                                &insights[INSIGHT_BYTES * ((i * 8) + j)],
                                private_key, state);
        }
    }

    shake256(statement, SECURITY_BITS / 8, message, message_length);
    memcpy(&statement[SECURITY_BITS / 8], public_key, PUBLIC_KEY_BYTES);
    sidh_pok_challenge(challenge_bytes, signature, COMMITMENT_BYTES * SECURITY_BITS, statement, sizeof(statement));

    for (uint8_t i = 0; i < SECURITY_BITS / 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            returned_value |= sidh_pok_response(
                    &signature[(COMMITMENT_BYTES * SECURITY_BITS) + (RESPONSE_BYTES * ((i * 8) + j))],
                    &insights[INSIGHT_BYTES * ((i * 8) + j)],
                    (challenge_bytes[i] >> j) & 1);
        }
    }

    return returned_value;
}

uint8_t sidh_signature_verify(const uint8_t signature[SIGNATURE_BYTES],
                              const uint8_t *message,
                              uint64_t message_length,
                              const uint8_t public_key[SECRET_KEY_BYTES]) {
    if (message_length == 0) {
        return EXIT_FAILURE;
    }

    uint8_t returned_value = 0;
    uint8_t challenge_bytes[SECURITY_BITS / 8] = {0};
    uint8_t statement[PUBLIC_KEY_BYTES + (SECURITY_BITS / 8)] = {0};

    shake256(statement, SECURITY_BITS / 8, message, message_length);
    memcpy(&statement[SECURITY_BITS / 8], public_key, PUBLIC_KEY_BYTES);
    sidh_pok_challenge(challenge_bytes, signature, COMMITMENT_BYTES * SECURITY_BITS, statement, sizeof(statement));

    for (uint8_t i = 0; i < SECURITY_BITS / 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            returned_value |= sidh_pok_verification(
                    &signature[COMMITMENT_BYTES * ((i * 8) + j)],
                    &signature[(COMMITMENT_BYTES * SECURITY_BITS) + (RESPONSE_BYTES * ((i * 8) + j))],
                    public_key,
                    (challenge_bytes[i] >> j) & 1);
        }
    }

    return returned_value;
}
