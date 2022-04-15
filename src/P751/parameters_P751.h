//
// Parameters concerning SIDHp751
//

#ifndef SIDH_POK_PARAMETERS_P751_H
#define SIDH_POK_PARAMETERS_P751_H

#define FIELD_NAME                  "p751"
#define FIELD_BITS                  751
#define FIELD_BYTES                 ((FIELD_BITS + 7) / 8)
#define SUBGROUP_ORDER_BITS         (FIELD_BITS - (FIELD_BITS / 2))
#define FIELD_64BITS_WORDS          ((FIELD_BITS + 63) / 64)
#define SUBGROUP_ORDER_64BITS_WORDS ((SUBGROUP_ORDER_BITS + 63) / 64)
#define QUADRATIC_FIELD_BYTES       (2 * FIELD_BYTES)
#define J_INVARIANT_BYTES           QUADRATIC_FIELD_BYTES
#define MASK_FIELD_ELEMENT          0x00007FFFFFFFFFFF
#define SECURITY_BITS               256

static const uint64_t PRIME_CHARACTERISTIC[FIELD_64BITS_WORDS] = {
        0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF,
        0xFFFFFFFFFFFFFFFF, 0xEEAFFFFFFFFFFFFF, 0xE3EC968549F878A8, 0xDA959B1A13F7CC76,
        0x084E9867D6EBE876, 0x8562B5045CB25748, 0x0E12909F97BADC66, 0x00006FE5D541F71C
};

// Montgomery constant R² = (2ᵉ)² mod PRIME_CHARACTERISTIC where e = 0 mod 64 s.t. 2ᵉ⁻⁶⁴ < PRIME_CHARACTERISTIC < 2ᵉ
static const uint64_t MONTGOMERY_CONSTANT_R_SQUARED[FIELD_64BITS_WORDS] = {
        0x233046449DAD4058, 0xDB010161A696452A, 0x5E36941472E3FD8E, 0xF40BFE2082A2E706,
        0x4932CCA8904F8751, 0x1F735F1F1EE7FC81, 0xA24F4D80C1048E18, 0xB56C383CCDB607C5,
        0x441DD47B735F9C90, 0x5673ED2C6A6AC82A, 0x06C905261132294B, 0x000041AD830F1F35
};

// Neutral multiplicative in Montgomery domain: R = 2ᵉ mod PRIME_CHARACTERISTIC
static const uint64_t MONTGOMERY_CONSTANT_ONE[FIELD_64BITS_WORDS] = {
        0x00000000000249ad, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x8310000000000000, 0x5527b1e4375c6c66, 0x697797bf3f4f24d0,
        0xc89db7b2ac5c4e2e, 0x4ca4b439d2076956, 0x10f7926c7512c7e9, 0x00002d5b24bce5e2
};

static const uint8_t TABLE_CANONICAL_BASIS[2][32] = {
        {0x2, 0x5, 0x8, 0x9, 0xA, 0xC, 0xD, 0x12, 0x13, 0x15, 0x16, 0x17, 0x1C, 0x1E,
                0x1F, 0x22, 0x23, 0x24, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x30, 0x31, 0x35, 0x36,
                0x37, 0x3C, 0x3D, 0x3F, 0x42},
        {0x1, 0x3, 0x4, 0x6, 0x7, 0xB, 0xE, 0xF,  0x10, 0x11, 0x14, 0x18, 0x19, 0x1A,
                0x1B, 0x1D, 0x20, 0x21, 0x25, 0x26, 0x27, 0x28, 0x2E, 0x2F, 0x32, 0x33, 0x34,
                0x38, 0x39, 0x3A, 0x3B, 0x3E}
};

// ++++++++++++++++++++++++++++++++++++
#define ALICE 1
#define EXPONENT_ALICE              372
#define SUBGROUP_ORDER_BITS_ALICE   372
#define SECRET_KEY_BYTES_ALICE      ((SUBGROUP_ORDER_BITS_ALICE + 7) / 8)
#define MASK_ALICE                  0x0F
#define MASK_ALICE_64BITS           (0x00FFFFFFFFFFFFFF ^ ((uint64_t)MASK_ALICE << 56))
#define STRATEGY_MAXIMUM_INTERNAL_POINTS_ALICE    8
#define NUMBER_OF_ISOGENIES_ALICE   186

// Alice's generators {XPA0 + XPA1*i, XQA0 + xQA1*i, XRA0 + XRA1*i} in GF(p^2), expressed in Montgomery representation
static const uint64_t PUBLIC_POINT_GENERATORS_ALICE[6 * FIELD_64BITS_WORDS] = {
        0x884F46B74000BAA8, 0xBA52630F939DEC20, 0xC16FB97BA714A04D, 0x082536745B1AB3DB,
        0x1117157F446F9E82, 0xD2F27D621A018490, 0x6B24AB523D544BCD, 0x9307D6AA2EA85C94,
        0xE1A096729528F20F, 0x896446F868F3255C, 0x2401D996B1BFF8A5, 0x00000EF8786A5C0A,     // XPA0
        0xAEB78B3B96F59394, 0xAB26681E29C90B74, 0xE520AC30FDC4ACF1, 0x870AAAE3A4B8111B,
        0xF875BDB738D64EFF, 0x50109A7ECD7ED6BC, 0x4CC64848FF0C56FB, 0xE617CB6C519102C9,
        0x9C74B3835921E609, 0xC91DDAE4A35A7146, 0x7FC82A155C1B9129, 0x0000214FA6B980B3,   // XPA1
        0x0F93CC38680A8CA9, 0x762E733822E7FED7, 0xE549F005AC0ADB67, 0x94A71FDD2C43A4ED,
        0xD48645C2B04721C5, 0x432DA1FE4D4CA4DC, 0xBC99655FAA7A80E8, 0xB2C6D502BCFD4823,
        0xEE92F40CA2EC8BDB, 0x7B074132EFB6D16C, 0x3340B46FA38A7633, 0x0000215749657F6C,   // XQA0
        0xECFF375BF3079F4C, 0xFBFE74B043E80EF3, 0x17376CBE3C5C7AD1, 0xC06327A7E29CDBF2,
        0x2111649C438BF3D4, 0xC1F9298261BA2E97, 0x1F9FECE869CFD1C2, 0x01A39B4FC9346D62,
        0x147CD1D3E82A3C9F, 0xDE84E9D249E533EE, 0x1C48A5ADFB7C578D, 0x000061ACA0B82E1D,   // XQA1
        0x1600C525D41059F1, 0xA596899A0A1D83F7, 0x6BFDEED6D2B23F35, 0x5C7E707270C23910,
        0x276CA1A4E8369411, 0xB193651A602925A0, 0x243D239F1CA1F04A, 0x543DC6DA457860AD,
        0xCDA590F325181DE9, 0xD3AB7ACFDA80B395, 0x6C97468580FDDF7B, 0x0000352A3E5C4C77,   // XRA0
        0x9B794F9FD1CC3EE8, 0xDB32E40A9B2FD23E, 0x26192A2542E42B67, 0xA18E94FCA045BCE7,
        0x96DC1BC38E7CDA2D, 0x9A1D91B752487DE2, 0xCC63763987436DA3, 0x1316717AACCC551D,
        0xC4C368A4632AFE72, 0x4B6EA85C9CCD5710, 0x7A12CAD582C7BC9A, 0x00001C7E240149BF    // XRA1
};

static const unsigned int STRATEGY_ALICE[NUMBER_OF_ISOGENIES_ALICE - 1] = {
        80, 48, 27, 15, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2,
        1, 1, 2, 1, 1, 7, 4, 2, 1, 1, 2, 1, 1,
        3, 2, 1, 1, 1, 1, 12, 7, 4, 2, 1, 1, 2,
        1, 1, 3, 2, 1, 1, 1, 1, 5, 3, 2, 1, 1,
        1, 1, 2, 1, 1, 1, 21, 12, 7, 4, 2, 1, 1,
        2, 1, 1, 3, 2, 1, 1, 1, 1, 5, 3, 2, 1,
        1, 1, 1, 2, 1, 1, 1, 9, 5, 3, 2, 1, 1,
        1, 1, 2, 1, 1, 1, 4, 2, 1, 1, 1, 2, 1,
        1, 33, 20, 12, 7, 4, 2, 1, 1, 2, 1, 1,
        3, 2, 1, 1, 1, 1, 5, 3, 2, 1, 1, 1,
        1, 2, 1, 1, 1, 8, 5, 3, 2, 1, 1, 1,
        1, 2, 1, 1, 1, 4, 2, 1, 1, 2, 1, 1,
        16, 8, 4, 2, 1, 1, 1, 2, 1, 1, 4, 2,
        1, 1, 2, 1, 1, 8, 4, 2, 1, 1, 2, 1,
        1, 4, 2, 1, 1, 2, 1, 1
};

// ++++++++++++++++++++++++++++++++++++
#define BOB 0
#define EXPONENT_BOB                239
#define SUBGROUP_ORDER_BITS_BOB     379
#define SECRET_KEY_BYTES_BOB        ((SUBGROUP_ORDER_BITS_BOB + 7) / 8)
#define MASK_BOB                    0x03
#define MASK_BOB_64BITS             (0x00FFFFFFFFFFFFFF ^ ((uint64_t)MASK_BOB << 56))
#define STRATEGY_MAXIMUM_INTERNAL_POINTS_BOB      10
#define NUMBER_OF_ISOGENIES_BOB     239

// Bob's generators {XPB0, XQB0, XRB0 + XRB1*i} in GF(p^2), expressed in Montgomery representation
static const uint64_t PUBLIC_POINT_GENERATORS_BOB[6 * FIELD_64BITS_WORDS] = {
        0x85691AAF4015F88C, 0x7478C5B8C36E9631, 0x7EF2A185DE4DD6E2, 0x943BBEE46BEB9DC7,
        0x1A3EC62798792D22, 0x791BC4B084B31D69, 0x03DBE6522CEA17C4, 0x04749AA65D665D83,
        0x3D52B5C45EF450F3, 0x0B4219848E36947D, 0xA4CF7070466BDE27, 0x0000334B1FA6D193,     // XPB0
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,   // XPB1
        0x8E7CB3FA53211340, 0xD67CE54F7A05EEE0, 0xFDDC2C8BCE46FC38, 0x08587FAE3110DF1E,
        0xD6B8246FA22B058B, 0x4DAC3ACC905A5DBD, 0x51D0BF2FADCED3E8, 0xE5A2406DF6484425,
        0x907F177584F671B8, 0x4738A2FFCCED051C, 0x2B0067B4177E4853, 0x00002806AC948D3D,   // XQB0
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,   // XQB1
        0xB56457016D1D6D1C, 0x03DECCB38F39C491, 0xDFB910AC8A559452, 0xA9D0F17D1FF24883,
        0x8562BBAF515C248C, 0x249B2A6DDB1CB67D, 0x3131AF96FB46835C, 0xE10258398480C3E1,
        0xEAB5E2B872D4FAB1, 0xB71E63875FAEB1DF, 0xF8384D4F13757CF6, 0x0000361EC9B09912,   // XRB0
        0x58C967899ED16EF4, 0x81998376DC622A4B, 0x3D1C1DCFE0B12681, 0x9347DEBB953E1730,
        0x9ABB344D3A82C2D7, 0xE4881BD2820552B2, 0x0037247923D90266, 0x2E3156EDB157E5A5,
        0xF86A46A7506823F7, 0x8FE5523A7B7F1CFC, 0xFA3CFFA38372F67B, 0x0000692DCE85FFBD    // XRB1
};

static const unsigned int STRATEGY_BOB[NUMBER_OF_ISOGENIES_BOB - 1] = {
        112, 63, 32, 16, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2,
        1, 1, 2, 1, 1, 8, 4, 2, 1, 1, 2, 1, 1,
        4, 2, 1, 1, 2, 1, 1, 16, 8, 4, 2, 1, 1,
        2, 1, 1, 4, 2, 1, 1, 2, 1, 1, 8, 4, 2,
        1, 1, 2, 1, 1, 4, 2, 1, 1, 2, 1, 1, 31,
        16, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2, 1, 1,
        2, 1, 1, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2,
        1, 1, 2, 1, 1, 15, 8, 4, 2, 1, 1, 2, 1,
        1, 4, 2, 1, 1, 2, 1, 1, 7, 4, 2, 1,
        1, 2, 1, 1, 3, 2, 1, 1, 1, 1, 49, 31,
        16, 8, 4, 2, 1, 1, 2, 1, 1, 4, 2, 1,
        1, 2, 1, 1, 8, 4, 2, 1, 1, 2, 1, 1,
        4, 2, 1, 1, 2, 1, 1, 15, 8, 4, 2, 1,
        1, 2, 1, 1, 4, 2, 1, 1, 2, 1, 1, 7,
        4, 2, 1, 1, 2, 1, 1, 3, 2, 1, 1, 1,
        1, 21, 12, 8, 4, 2, 1, 1, 2, 1, 1, 4,
        2, 1, 1, 2, 1, 1, 5, 3, 2, 1, 1, 1,
        1, 2, 1, 1, 1, 9, 5, 3, 2, 1, 1, 1,
        1, 2, 1, 1, 1, 4, 2, 1, 1, 1, 2, 1,
        1
};

static const unsigned int STRATEGY_POHLIG_HELLMAN[NUMBER_OF_ISOGENIES_BOB - 1] = {
        144, 56, 22, 9, 5, 2, 1, 3, 1, 2, 1, 5, 2, 1,
        1, 3, 1, 2, 1, 13, 5, 2, 1, 1, 3, 1, 2,
        1, 8, 3, 1, 2, 1, 5, 2, 1, 3, 1, 2, 1,
        34, 13, 5, 2, 1, 1, 3, 1, 2, 1, 8, 3, 1,
        2, 1, 5, 2, 1, 3, 1, 2, 1, 21, 8, 3, 1,
        2, 1, 5, 2, 1, 3, 1, 2, 1, 13, 5, 2, 1,
        3, 1, 2, 1, 8, 3, 1, 2, 1, 5, 2, 1, 3,
        1, 2, 1, 88, 34, 13, 5, 2, 1, 1, 3, 1,
        2, 1, 8, 3, 1, 2, 1, 5, 2, 1, 3, 1,
        2, 1, 21, 8, 3, 1, 2, 1, 5, 2, 1, 3,
        1, 2, 1, 13, 5, 2, 1, 3, 1, 2, 1, 8,
        3, 1, 2, 1, 5, 2, 1, 3, 1, 2, 1, 54,
        21, 8, 3, 1, 2, 1, 5, 2, 1, 3, 1, 2,
        1, 13, 5, 2, 1, 3, 1, 2, 1, 8, 3, 1,
        2, 1, 5, 2, 1, 3, 1, 2, 1, 33, 13, 5,
        2, 1, 3, 1, 2, 1, 8, 3, 1, 2, 1, 5,
        2, 1, 3, 1, 2, 1, 20, 8, 3, 1, 2, 1,
        5, 2, 1, 3, 1, 2, 1, 12, 5, 2, 1, 3,
        1, 2, 1, 7, 3, 1, 2, 1, 4, 2, 1, 2,
        1, 1
};

#endif //SIDH_POK_PARAMETERS_P751_H