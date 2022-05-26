//
// Parameters concerning SIDHp503
//

#ifndef SIDH_POK_PARAMETERS_P503_H
#define SIDH_POK_PARAMETERS_P503_H

#define FIELD_NAME                  "p503"
#define FIELD_BITS                  503
#define FIELD_BYTES                 ((FIELD_BITS + 7) / 8)
#define SUBGROUP_ORDER_BITS         (FIELD_BITS - (FIELD_BITS / 2))
#define FIELD_64BITS_WORDS          ((FIELD_BITS + 63) / 64)
#define SUBGROUP_ORDER_64BITS_WORDS ((SUBGROUP_ORDER_BITS + 63) / 64)
#define QUADRATIC_FIELD_BYTES       (2 * FIELD_BYTES)
#define J_INVARIANT_BYTES           QUADRATIC_FIELD_BYTES
#define MASK_FIELD_ELEMENT          0x007FFFFFFFFFFFFF
#define SECURITY_BITS               160

static const uint64_t PRIME_CHARACTERISTIC[FIELD_64BITS_WORDS] = {
        0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xABFFFFFFFFFFFFFF,
        0x13085BDA2211E7A0, 0x1B9BF6C87B7E7DAF, 0x6045C6BDDA77A4D0, 0x004066F541811E1E
};

// Montgomery constant R² = (2ᵉ)² mod PRIME_CHARACTERISTIC where e = 0 mod 64 s.t. 2ᵉ⁻⁶⁴ < PRIME_CHARACTERISTIC < 2ᵉ
static const uint64_t MONTGOMERY_CONSTANT_R_SQUARED[FIELD_64BITS_WORDS] = {
        0x5289A0CF641D011F, 0x9B88257189FED2B9, 0xA3B365D58DC8F17A, 0x5BC57AB6EFF168EC,
        0x9E51998BD84D4423, 0xBF8999CBAC3B5695, 0x46E9127BCE14CDB6, 0x003F6CFCE8B81771
};

// Neutral multiplicative in Montgomery domain: R = 2ᵉ mod PRIME_CHARACTERISTIC
static const uint64_t MONTGOMERY_CONSTANT_ONE[FIELD_64BITS_WORDS] = {
        0x00000000000003F9, 0x0000000000000000, 0x0000000000000000, 0xB400000000000000,
        0x63CB1A6EA6DED2B4, 0x51689D8D667EB37D, 0x8ACD77C71AB24142, 0x0026FBAEC60F5953
};

static const uint16_t TABLE_CANONICAL_BASIS[2][128] = {
        { 0x1, 0x2, 0x5, 0x7, 0x8, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x14, 0x15, 0x17, 0x19, 0x1A, 0x1C, 0x1F, 0x21, 0x23, 0x2A, 0x2C, 0x2F, 0x31, 0x33, 0x36, 0x39, 0x3F, 0x43, 0x44, 0x45, 0x47, 0x48, 0x49, 0x4B, 0x4D, 0x4E, 0x4F, 0x51, 0x52, 0x53,
     0x57, 0x58, 0x59, 0x5B, 0x5C, 0x5E, 0x5F, 0x61, 0x63, 0x64, 0x65, 0x66, 0x68, 0x69, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x71, 0x72, 0x73, 0x76, 0x78, 0x7B, 0x7C, 0x7F, 0x81, 0x83, 0x84, 0x85, 0x86, 0x87, 0x89, 0x8A, 0x8C, 0x90, 0x92, 0x93, 0x94, 0x95,
     0x96, 0x98, 0x9A, 0x9F, 0xA1, 0xA3, 0xA4, 0xA9, 0xAA, 0xAB, 0xAC, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB8, 0xBA, 0xBE, 0xBF, 0xC2, 0xC5, 0xC6, 0xC9, 0xCB, 0xCC, 0xCF, 0xD0, 0xD4, 0xD8, 0xD9, 0xDA, 0xDC, 0xE0, 0xE1, 0xE2, 0xE3, 0xE5, 0xE6, 0xE7,
     0xE8, 0xEA, 0xEB },
        { 0x3, 0x4, 0x6, 0x9, 0x12, 0x13, 0x16, 0x18, 0x1B, 0x1D, 0x1E, 0x20, 0x22, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2B, 0x2D, 0x2E, 0x30, 0x32, 0x34, 0x35, 0x37, 0x38, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x40, 0x41, 0x42, 0x46, 0x4A, 0x4C, 0x50, 0x54, 0x55,
     0x56, 0x5A, 0x5D, 0x60, 0x62, 0x67, 0x6A, 0x70, 0x74, 0x75, 0x77, 0x79, 0x7A, 0x7D, 0x7E, 0x80, 0x82, 0x88, 0x8B, 0x8D, 0x8E, 0x8F, 0x91, 0x97, 0x99, 0x9B, 0x9C, 0x9D, 0x9E, 0xA0, 0xA2, 0xA5, 0xA6, 0xA7, 0xA8, 0xAD, 0xAE, 0xB5, 0xB6, 0xB7, 0xB9,
     0xBB, 0xBC, 0xBD, 0xC0, 0xC1, 0xC3, 0xC4, 0xC7, 0xC8, 0xCA, 0xCD, 0xCE, 0xD1, 0xD2, 0xD3, 0xD5, 0xD6, 0xD7, 0xDB, 0xDD, 0xDE, 0xDF, 0xE4, 0xE9, 0xEC, 0xED, 0xEE, 0xF0, 0xF1, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0x100, 0x102, 0x103, 0x105, 0x107,
     0x109, 0x10C, 0x10E, 0x112, 0x113 }
};

// ++++++++++++++++++++++++++++++++++++
#define ALICE 1
#define EXPONENT_ALICE              250
#define SUBGROUP_ORDER_BITS_ALICE   250
#define SECRET_KEY_BYTES_ALICE      ((SUBGROUP_ORDER_BITS_ALICE + 7) / 8)
#define MASK_ALICE                  0x03
#define MASK_ALICE_64BITS           (0x00FFFFFFFFFFFFFF ^ ((uint64_t)MASK_ALICE << 56))
#define STRATEGY_MAXIMUM_INTERNAL_POINTS_ALICE    7
#define NUMBER_OF_ISOGENIES_ALICE   125

// Alice's generators {XPA0 + XPA1*i, XQA0 + xQA1*i, XRA0 + XRA1*i} in GF(p^2), expressed in Montgomery representation
static const uint64_t PUBLIC_POINT_GENERATORS_ALICE[6 * FIELD_64BITS_WORDS] = {
        0x5D083011589AD893, 0xADFD8D2CB67D0637, 0x330C9AC34FFB6361, 0xF0D47489A2E805A2,
        0x27E2789259C6B8DC, 0x63866A2C121931B9, 0x8D4C65A7137DCF44, 0x003A183AE5967B3F,   // XPA0
        0x7E3541B8C96D1519, 0xD3ADAEEC0D61A26C, 0xC0A2219CE7703DD9, 0xFF3E46658FCDBC52,
        0xD5B38DEAE6E196FF, 0x1AAC826364956D58, 0xEC9F4875B9A5F27A, 0x001B0B475AB99843,   // XPA1
        0x4D83695107D03BAD, 0x221F3299005E2FCF, 0x78E6AE22F30DECF2, 0x6D982DB5111253E4,
        0x504C80A8AB4526A8, 0xEFD0C3AA210BB024, 0xCB77483501DC6FCF, 0x001052544A96BDF3,   // XQA0
        0x0D74FE3402BCAE47, 0xDF5B8CDA832D8AED, 0xB86BCF06E4BD837E, 0x892A2933A0FA1F63,
        0x9F88FC67B6CCB461, 0x822926EA9DDA3AC8, 0xEAC8DDE5855425ED, 0x000618FE6DA37A80,   // XQA1
        0x1D9D32D2DC877C17, 0x5517CD8F71D5B02B, 0x395AFB8F6B60C117, 0x3AE31AC85F9098C8,
        0x5F5341C198450848, 0xF8C609DBEA435C6A, 0xD832BC7EDC7BA5E4, 0x002AD98AA6968BF5,   // XRA0
        0xC466CAB0F73C2E5B, 0x7B1817148FB2CF9C, 0x873E87C099E470A0, 0xBB17AC6D17A7BAC1,
        0xA146FDCD0F2E2A58, 0x88B311E9CEAB6201, 0x37604CF5C7951757, 0x0006804071C74BF9    // XRA1
};

static const unsigned int STRATEGY_ALICE[NUMBER_OF_ISOGENIES_ALICE - 1] = {
        61, 32, 16, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2, 1,
        1, 2, 1, 1, 8, 4, 2, 1, 1, 2, 1, 1, 4,
        2, 1, 1, 2, 1, 1, 16, 8, 4, 2, 1, 1, 2,
        1, 1, 4, 2, 1, 1, 2, 1, 1, 8, 4, 2, 1,
        1, 2, 1, 1, 4, 2, 1, 1, 2, 1, 1, 29, 16,
        8, 4, 2, 1, 1, 2, 1, 1, 4, 2, 1, 1, 2,
        1, 1, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2, 1,
        1, 2, 1, 1, 13, 8, 4, 2, 1, 1, 2, 1,
        1, 4, 2, 1, 1, 2, 1, 1, 5, 4, 2, 1,
        1, 2, 1, 1, 2, 1, 1, 1
};

// ++++++++++++++++++++++++++++++++++++
#define BOB 0
#define EXPONENT_BOB                159
#define SUBGROUP_ORDER_BITS_BOB     253
#define SECRET_KEY_BYTES_BOB        ((SUBGROUP_ORDER_BITS_BOB + 7) / 8)
#define MASK_BOB                    0x0F
#define MASK_BOB_64BITS             (0x00FFFFFFFFFFFFFF ^ ((uint64_t)MASK_BOB << 56))
#define STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB      8
#define NUMBER_OF_ISOGENIES_BOB     159

// Bob's generators {XPB0, XQB0, XRB0 + XRB1*i} in GF(p^2), expressed in Montgomery representation
static const uint64_t PUBLIC_POINT_GENERATORS_BOB[6 * FIELD_64BITS_WORDS] = {
        0xDF630FC5FB2468DB, 0xC30C5541C102040E, 0x3CDC9987B76511FC, 0xF54B5A09353D0CDD,
        0x3ADBA8E00703C42F, 0x8253F9303DDC95D0, 0x62D30778763ABFD7, 0x001CD00FB581CD55,   // XPB0
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,   // XPB1
        0x2E3457A12B429261, 0x311F94E89627DCF8, 0x5B71C98FD1DB73F6, 0x3671DB7DCFC21541,
        0xB6D1484C9FE0CF4F, 0x19CD110717356E35, 0xF4F9FB00AC9919DF, 0x0035BC124D38A70B,   // XQB0
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,   // XQB1
        0x2E08BB99413D2952, 0xD3021467CD088D72, 0x21017AF859752245, 0x26314ED8FFD9DE5C,
        0x4AF43C73344B6686, 0xCFA1F91149DF0993, 0xF327A95365587A89, 0x000DBF54E03D3906,   // XRB0
        0x03E03FF342F5F304, 0x993D604D7B4B6E56, 0x80412F4D9280E71F, 0x0FFDC9EF990B3982,
        0xE584E64C51604931, 0x1374F42AC8B0BBD7, 0x07D5BC37DFA41A5F, 0x00396CCFD61FD34C    // XRB1
};

static const unsigned int STRATEGY_BOB[NUMBER_OF_ISOGENIES_BOB - 1] = {
        71, 38, 21, 13, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2,
        1, 1, 2, 1, 1, 5, 4, 2, 1, 1, 2, 1, 1,
        2, 1, 1, 1, 9, 5, 3, 2, 1, 1, 1, 1, 2,
        1, 1, 1, 4, 2, 1, 1, 1, 2, 1, 1, 17, 9,
        5, 3, 2, 1, 1, 1, 1, 2, 1, 1, 1, 4, 2,
        1, 1, 1, 2, 1, 1, 8, 4, 2, 1, 1, 1, 2,
        1, 1, 4, 2, 1, 1, 2, 1, 1, 33, 17, 9, 5,
        3, 2, 1, 1, 1, 1, 2, 1, 1, 1, 4, 2, 1,
        1, 1, 2, 1, 1, 8, 4, 2, 1, 1, 1, 2,
        1, 1, 4, 2, 1, 1, 2, 1, 1, 16, 8, 4,
        2, 1, 1, 1, 2, 1, 1, 4, 2, 1, 1, 2,
        1, 1, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2,
        1, 1, 2, 1, 1
};

static const unsigned int STRATEGY_POHLIG_HELLMAN[NUMBER_OF_ISOGENIES_BOB - 1] = {
        93, 38, 16, 7, 3, 1, 2, 1, 4, 2, 1, 2, 1, 1,
        9, 4, 2, 1, 2, 1, 1, 5, 2, 1, 1, 3, 1,
        2, 1, 22, 9, 4, 2, 1, 2, 1, 1, 5, 2, 1,
        1, 3, 1, 2, 1, 13, 5, 2, 1, 1, 3, 1, 2,
        1, 8, 3, 1, 2, 1, 5, 2, 1, 3, 1, 2, 1,
        55, 22, 9, 4, 2, 1, 2, 1, 1, 5, 2, 1, 1,
        3, 1, 2, 1, 13, 5, 2, 1, 1, 3, 1, 2, 1,
        8, 3, 1, 2, 1, 5, 2, 1, 3, 1, 2, 1,
        33, 13, 5, 2, 1, 1, 3, 1, 2, 1, 8,
        3, 1, 2, 1, 5, 2, 1, 3, 1, 2, 1, 20,
        8, 3, 1, 2, 1, 5, 2, 1, 3, 1, 2, 1,
        12, 5, 2, 1, 3, 1, 2, 1, 7, 3, 1, 2,
        1, 4, 2, 1, 2, 1, 1
};

#endif //SIDH_POK_PARAMETERS_P503_H

