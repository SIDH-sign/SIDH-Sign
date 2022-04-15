//
// Digital signature scheme based on PoK
//

#ifndef SIDH_POK_SIGNATURE_H
#define SIDH_POK_SIGNATURE_H

#include <stdint.h>
#include "pok.h"

#define SECRET_KEY_BYTES    SECRET_KEY_BYTES_ALICE
#define SIGNATURE_BYTES     ((COMMITMENT_BYTES + RESPONSE_BYTES) * SECURITY_BITS)

uint8_t sidh_signature_key_generation(uint8_t private_key_bytes[SECRET_KEY_BYTES],
                                      uint8_t public_key_bytes[PUBLIC_KEY_BYTES],
                                      keccak_state *state);

uint8_t sidh_signature_sign(uint8_t signature[SIGNATURE_BYTES],
                            const uint8_t *message,
                            uint64_t message_length,
                            const uint8_t private_key[SECRET_KEY_BYTES],
                            const uint8_t public_key[PUBLIC_KEY_BYTES],
                            keccak_state *state);

uint8_t sidh_signature_verify(const uint8_t signature[SIGNATURE_BYTES],
                              const uint8_t *message,
                              uint64_t message_length,
                              const uint8_t public_key[SECRET_KEY_BYTES]);

#endif //SIDH_POK_SIGNATURE_H
