//
// Proof Of Knowledge (PoK)
// SIDH built-in functions
//

#ifndef SIDH_POK_POK_H
#define SIDH_POK_POK_H

#include "sidh.h"
#include "twisted_edwards.h"

#define COMMITMENT_BYTES    (QUADRATIC_FIELD_BYTES + PUBLIC_KEY_BYTES)   // {E₂, P₃, Q₃, P₃-Q₃}
#define INSIGHT_BYTES       (2*QUADRATIC_FIELD_BYTES + COMMITMENT_BYTES) // {ker φ', ker (dual of ψ), E₂, P₂, Q₂, P₂-Q₂}
#define RESPONSE_BYTES      QUADRATIC_FIELD_BYTES    // either ker φ' or decomposition by scalars (c, d) of the dual of ψ

// *************** INTERNAL FUNCTIONS *******************//

void random_private_key_sample(uint8_t output[SECRET_KEY_BYTES_BOB + 1], keccak_state *state);

uint8_t canonical_basis(sidh_public_key_t *output_basis, quadratic_field_element_t input_A);

// Next function should use to decompose the kernel of the dual isogeny
uint8_t decomposition_by_scalars(scalar_t output_c,
                                 scalar_t output_d,
                                 quadratic_field_element_t input_kernel,
                                 sidh_public_key_t input_basis,
                                 quadratic_field_element_t input_A);


// ****************** EXTERNAL API *******************//


// The knowledge is Alice's private key
uint8_t sidh_pok_commitment(uint8_t commit[384], uint8_t insight[576], const uint8_t private_key_alice[24],
                         keccak_state *state);

void sidh_pok_challenge(uint8_t *challenge, const uint8_t *commitment, uint64_t commitment_size,
                        const uint8_t *statement, uint64_t statement_size);

uint8_t sidh_pok_response(uint8_t resp[RESPONSE_BYTES], const uint8_t insight[INSIGHT_BYTES], uint8_t challenge);

uint8_t sidh_pok_verification(const uint8_t commit[COMMITMENT_BYTES],
                              const uint8_t resp[RESPONSE_BYTES],
                              const uint8_t public_key_alice[PUBLIC_KEY_BYTES],
                              uint8_t challenge);

#endif //SIDH_POK_POK_H

