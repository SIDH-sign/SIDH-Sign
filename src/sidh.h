//
// SIDH built-in functions
//

#ifndef SIDH_POK_SIDH_H
#define SIDH_POK_SIDH_H

#include "isogeny.h"

#define PUBLIC_KEY_BYTES    (3*QUADRATIC_FIELD_BYTES)

typedef struct {
    quadratic_field_element_t P;
    quadratic_field_element_t Q;
    quadratic_field_element_t P_minus_Q;
} sidh_public_key_t;

typedef struct {
    scalar_t key;
} sidh_private_key_t;

typedef struct {
    quadratic_field_element_t curve;
} sidh_shared_secret_t; // j-invariant not required (same curves)


void simultaneous_three_inverses(quadratic_field_element_t *input_output_z1,
                                 quadratic_field_element_t *input_output_z2,
                                 quadratic_field_element_t *input_output_z3);

void get_curve_from_points(quadratic_field_element_t *output,
                           quadratic_field_element_t input_P,
                           quadratic_field_element_t input_Q,
                           quadratic_field_element_t input_P_minus_Q);

void sidh_generate_private_key_alice(sidh_private_key_t *private_key_alice, keccak_state *state);

void sidh_get_public_key_from_private_key_alice(sidh_public_key_t *public_key_alice,
                                                const sidh_private_key_t *private_key_alice);

void sidh_key_agreement_alice(sidh_shared_secret_t *shared_secret_alice,
                              const sidh_private_key_t *private_key_alice,
                              const sidh_public_key_t *public_key_bob);

void sidh_generate_private_key_bob(sidh_private_key_t *private_key_bob, keccak_state *state);

void sidh_get_public_key_from_private_key_bob(sidh_public_key_t *public_key_bob,
                                              const sidh_private_key_t *private_key_bob);

void sidh_key_agreement_bob(sidh_shared_secret_t *shared_secret_bob,
                            const sidh_private_key_t *private_key_bob,
                            const sidh_public_key_t *public_key_alice);

void sidh_public_key_to_bytes(uint8_t pk_bytes[PUBLIC_KEY_BYTES], const sidh_public_key_t *pk);

void sidh_public_key_from_bytes(sidh_public_key_t *pk, const uint8_t pk_bytes[PUBLIC_KEY_BYTES]);

void sidh_private_key_to_bytes_bob(uint8_t sk_bytes[(SUBGROUP_ORDER_BITS + 7) / 8], const sidh_private_key_t *sk);

void sidh_private_key_from_bytes_bob(sidh_private_key_t *sk, const uint8_t sk_bytes[(SUBGROUP_ORDER_BITS + 7) / 8]);

void sidh_private_key_to_bytes_alice(uint8_t sk_bytes[(SUBGROUP_ORDER_BITS + 7) / 8], const sidh_private_key_t *sk);

void sidh_private_key_from_bytes_alice(sidh_private_key_t *sk, const uint8_t sk_bytes[(SUBGROUP_ORDER_BITS + 7) / 8]);

#endif //SIDH_POK_SIDH_H
