//
// Parameters concerning SIDHp377
// word_by_word_montgomery 'p377' '64' '0x15b702e0c542196a879cc6988ce7cf50b46d546bc2a56997fffffffffffffffffffffffffffffffffffffffffffffff'
//

#ifndef SIDH_POK_PARAMETERS_P377_H
#define SIDH_POK_PARAMETERS_P377_H

#define FIELD_NAME                  "p377"
#define FIELD_BITS                  377
#define FIELD_BYTES                 ((FIELD_BITS + 7) / 8)
#define SUBGROUP_ORDER_BITS         (FIELD_BITS - (FIELD_BITS / 2))
#define FIELD_64BITS_WORDS          ((FIELD_BITS + 63) / 64)
#define SUBGROUP_ORDER_64BITS_WORDS ((SUBGROUP_ORDER_BITS + 63) / 64)
#define QUADRATIC_FIELD_BYTES       (2 * FIELD_BYTES)
#define J_INVARIANT_BYTES           QUADRATIC_FIELD_BYTES
#define MASK_FIELD_ELEMENT          0x01ffffffffffffff
#define SECURITY_BITS               128

static const uint64_t PRIME_CHARACTERISTIC[FIELD_64BITS_WORDS] = {
        0xffffffffffffffff, 0xffffffffffffffff, 0x7fffffffffffffff,
        0x0b46d546bc2a5699, 0xa879cc6988ce7cf5, 0x015b702e0c542196
};

// Montgomery constant R² = (2ᵉ)² mod PRIME_CHARACTERISTIC where e = 0 mod 64 s.t. 2ᵉ⁻⁶⁴ < PRIME_CHARACTERISTIC < 2ᵉ
static const uint64_t MONTGOMERY_CONSTANT_R_SQUARED[FIELD_64BITS_WORDS] = {
        0x826e131d3839c923, 0x54892c7b7d73e7f7, 0x3f8957d221b867a3,
        0xd1217cd71d03bb94, 0xdccbfb71e3ae5457, 0x00fcc56b6cd4b219
};

// Neutral multiplicative in Montgomery domain: R = 2ᵉ mod PRIME_CHARACTERISTIC
static const uint64_t MONTGOMERY_CONSTANT_ONE[FIELD_64BITS_WORDS] = {
        0x00000000000000bc, 0x0000000000000000, 0x0000000000000000,
        0xb7fb600dd0e86746, 0x468de27f885c3c0b, 0x00d99e2ef237555c,
};

static const uint16_t TABLE_CANONICAL_BASIS[2][128] = {
    { 0x1, 0x2, 0x5, 0x8, 0xB, 0xD, 0xE, 0x12, 0x14, 0x18, 0x19, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x22, 0x25, 0x29, 0x2B, 0x2D, 0x2E, 0x30, 0x33, 0x34, 0x35, 0x39, 0x3A, 0x3B, 0x3F, 0x41, 0x42, 0x43, 0x44, 0x47, 0x48, 0x49, 0x4A, 0x4D, 0x4F, 0x50,
     0x51, 0x52, 0x54, 0x58, 0x59, 0x5A, 0x5C, 0x5E, 0x60, 0x61, 0x63, 0x68, 0x69, 0x6A, 0x6C, 0x6D, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x7A, 0x7B, 0x7C, 0x7E, 0x7F, 0x80, 0x85, 0x89, 0x8A, 0x8E, 0x91, 0x93, 0x94, 0x95, 0x97,
     0x98, 0x99, 0x9B, 0x9C, 0x9F, 0xA1, 0xA2, 0xA6, 0xA8, 0xAA, 0xAD, 0xB3, 0xB4, 0xB7, 0xBB, 0xBD, 0xBE, 0xBF, 0xC0, 0xC2, 0xC3, 0xC4, 0xC7, 0xC8, 0xCA, 0xCC, 0xCF, 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDB, 0xDC, 0xDE, 0xDF, 0xE1, 0xE2, 0xE3, 0xE8,
     0xEB, 0xED, 0xEF, 0xF2 },
    { 0x3, 0x4, 0x6, 0x7, 0x9, 0xA, 0xC, 0xF, 0x10, 0x11, 0x13, 0x15, 0x16, 0x17, 0x1A, 0x21, 0x23, 0x24, 0x26, 0x27, 0x28, 0x2A, 0x2C, 0x2F, 0x31, 0x32, 0x36, 0x37, 0x38, 0x3C, 0x3D, 0x3E, 0x40, 0x45, 0x46, 0x4B, 0x4C, 0x4E, 0x53, 0x55, 0x56, 0x57,
     0x5B, 0x5D, 0x5F, 0x62, 0x64, 0x65, 0x66, 0x67, 0x6B, 0x6E, 0x79, 0x7D, 0x81, 0x82, 0x83, 0x84, 0x86, 0x87, 0x88, 0x8B, 0x8C, 0x8D, 0x8F, 0x90, 0x92, 0x96, 0x9A, 0x9D, 0x9E, 0xA0, 0xA3, 0xA4, 0xA5, 0xA7, 0xA9, 0xAB, 0xAC, 0xAE, 0xAF, 0xB0, 0xB1,
     0xB2, 0xB5, 0xB6, 0xB8, 0xB9, 0xBA, 0xBC, 0xC1, 0xC5, 0xC6, 0xC9, 0xCB, 0xCD, 0xCE, 0xD1, 0xD3, 0xD5, 0xD7, 0xD9, 0xDD, 0xE0, 0xE4, 0xE5, 0xE6, 0xE7, 0xE9, 0xEA, 0xEC, 0xEE, 0xF0, 0xF1, 0xF4, 0xF6, 0xF9, 0xFF, 0x100, 0x103, 0x105, 0x107, 0x10D,
     0x10E, 0x10F, 0x111, 0x113, 0x115 }
};

// ++++++++++++++++++++++++++++++++++++
#define ALICE 1
#define EXPONENT_ALICE              191
#define SUBGROUP_ORDER_BITS_ALICE   191
#define SECRET_KEY_BYTES_ALICE      ((SUBGROUP_ORDER_BITS_ALICE + 7) / 8)
#define MASK_ALICE                  0x7F
#define MASK_ALICE_64BITS           (0x00FFFFFFFFFFFFFF ^ ((uint64_t)MASK_ALICE << 56))
#define STRATEGY_MAXIMUM_INTERNAL_POINTS_ALICE    7
#define NUMBER_OF_ISOGENIES_ALICE   95
#define SECURITY_BITS               128

// Alice's generators {XPA0 + XPA1*i, XQA0 + xQA1*i, XRA0 + XRA1*i} in GF(p^2), expressed in Montgomery representation
static const uint64_t PUBLIC_POINT_GENERATORS_ALICE[6 * FIELD_64BITS_WORDS] = {
        0x8AE392AA8312F880, 0xDB7F6BA38CC56011, 0x896F67240AD52C67,
        0x21B9C0BD6C0584FF, 0xF064B97DDD0B2BD4, 0x0102EA98B786D4CC,   // XPA0
        0x583DE90ED3D09845, 0x131B1BDFBBE25620, 0x054B16A62F3D59F1,
        0x1C3A458EEFFD4A0B, 0x1FBC000608BE1F7A, 0x00225F4BEEF34209,   // XPA1
        0x8AA130E98FE00DE5, 0x6B54CC5A0A538778, 0x46D96D4F04F6605D,
        0x069A3CAB971973AE, 0x8923D0F2112DA219, 0x0085C1C47AD21A2A,   // XQA0
        0x50981EA202812D84, 0x61883F048CF1682A, 0x2DBC9EC88567E391,
        0xD5E238E99DD189E7, 0x1BFE095BC910EA7D, 0x00203E87957453EB,   // XQA1
        0x296CA63890082DB3, 0x02E16D4D70C2C55A, 0xD4B8FE9CB9481E99,
        0xF95F9798C3BECDFB, 0x71B3A2D8A38CB84B, 0x0118DD7682525B04,   // XRA0
        0xF64DD26CEC6E9DF5, 0xBC02B5979FF4F94C, 0x5D8B16849129DE49,
        0xE44435C64BEFB9E9, 0x1077D183B5A4727B, 0x0019A2DF755CF268    // XRA1
};

static const unsigned int STRATEGY_ALICE[NUMBER_OF_ISOGENIES_ALICE - 1] = {
        38, 26, 15, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2, 1,
        1, 2, 1, 1, 7, 4, 2, 1, 1, 2, 1, 1, 3, 2, 1,
        1, 1, 1, 11, 7, 4, 2, 1, 1, 2, 1, 1, 3, 2, 1,
        1, 1, 1, 4, 3, 2, 1, 1, 1, 1, 2, 1, 1, 17,
        9, 5, 3, 2, 1, 1, 1, 1, 2, 1, 1, 1, 4, 2, 1,
        1, 1, 2, 1, 1, 8, 4, 2, 1, 1, 1, 2, 1, 1, 4,
        2, 1, 1, 2, 1, 1
};

// ++++++++++++++++++++++++++++++++++++
#define BOB 0
#define EXPONENT_BOB                117
#define SUBGROUP_ORDER_BITS_BOB     186
#define SECRET_KEY_BYTES_BOB        ((SUBGROUP_ORDER_BITS_BOB + 7) / 8)
#define MASK_BOB                    0x01
#define MASK_BOB_64BITS             (0x00FFFFFFFFFFFFFF ^ ((uint64_t)MASK_BOB << 56))
#define STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB      8
#define NUMBER_OF_ISOGENIES_BOB     117

// Bob's generators {XPB0, XQB0, XRB0 + XRB1*i} in GF(p^2), expressed in Montgomery representation
static const uint64_t PUBLIC_POINT_GENERATORS_BOB[6 * FIELD_64BITS_WORDS] = {
        0x436424EE3C9446F8, 0xB013A914D96E976D, 0x30C376697D926658,
        0xE99792AFAA115E68, 0x935421EF522A946B, 0x0032474AECB8799E,   // XPB0 
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,   // XPB1
        0x5EDE445E538850BC, 0x5BA7DAD976595394, 0xF01F46B8519CD118,
        0x9DFA5CB5B40775A1, 0xC7E535F99811B56B, 0x0025BF8D8B00A170,   // XQB0
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,   // XQB1
        0xA35AA9C8EA887C42, 0xE5A1BF165361C81A, 0x719BB1C6D6C727C7,
        0x348590861EB46882, 0xB57273062A50C238, 0x002C53E0163A1C34,   // XRB0
        0xF12E87A9F00803D8, 0x49C966997253584C, 0x58BBD82219B363ED,
        0x6232DFE1A85929F5, 0xC85434A71BF3CC30, 0x005DE7FAB257510D    // XRB1
};

static const unsigned int STRATEGY_BOB[NUMBER_OF_ISOGENIES_BOB - 1] = {
        54, 31, 16, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2, 1, 1, 2, 1, 1,
        8, 4, 2, 1, 1, 2, 1, 1, 4, 2, 1, 1, 2, 1, 1, 15, 8, 4, 2,
        1, 1, 2, 1, 1, 4, 2, 1, 1, 2, 1, 1, 7, 4, 2, 1, 1, 2, 1,
        1, 3, 2, 1, 1, 1, 1, 23, 15, 8, 4, 2, 1, 1, 2, 1, 1, 4,
        2, 1, 1, 2, 1, 1, 7, 4, 2, 1, 1, 2, 1, 1, 3, 2, 1, 1, 1,
        1, 9, 6, 4, 2, 1, 1, 2, 1, 1, 2, 2, 1, 1, 1, 4, 2, 1, 1,
        1, 2, 1, 1
};

static const unsigned int STRATEGY_POHLIG_HELLMAN[NUMBER_OF_ISOGENIES_BOB - 1] = {
        67, 30, 12, 5, 2, 1, 3, 1, 2, 1, 7, 3, 1, 2, 1, 4, 2, 1,
        2, 1, 1, 17, 8, 3, 1, 2, 1, 5, 2, 1, 3, 1, 2, 1, 9, 5, 2,
        1, 3, 1, 2, 1, 5, 2, 1, 1, 3, 1, 2, 1, 39, 16, 7, 3, 1,
        2, 1, 4, 2, 1, 2, 1, 1, 9, 4, 2, 1, 2, 1, 1, 5, 2, 1, 1,
        3, 1, 2, 1, 22, 9, 5, 2, 1, 3, 1, 2, 1, 5, 2, 1, 1, 3, 1,
        2, 1, 13, 5, 2, 1, 1, 3, 1, 2, 1, 8, 3, 1, 2, 1, 5, 2, 1,
        3, 1, 2, 1
};

#endif //SIDH_POK_PARAMETERS_P377_H

