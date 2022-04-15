.intel_syntax noprefix

// Format function and variable names for Mac OS X
#if defined(__APPLE__)
	#define fmt(f)	_##f
#else
	#define fmt(f)	f
#endif

#define reg_p1  rdi
#define reg_p2  rsi
#define reg_p3  rdx

.global prime_modulus_p1
prime_modulus_p1:
    .quad 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0xAC00000000000000
    .quad 0x13085BDA2211E7A0, 0x1B9BF6C87B7E7DAF, 0x6045C6BDDA77A4D0, 0x004066F541811E1E


.global prime_modulus_x4
prime_modulus_x4:
    .quad 0xFFFFFFFFFFFFFFFC, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xAFFFFFFFFFFFFFFF
    .quad 0x4C216F6888479E82, 0x6E6FDB21EDF9F6BC, 0x81171AF769DE9340, 0x01019BD506047879

.text
.P2ALIGN 4,,15

.macro SWAP2, R0, R1, T0
	xor \R0, \R1
	and \R0, \T0
	xor \R1, \R0
.endm

.macro REDUCE512 M, R0, R1, R2, R3, R4, R5, R6, R7, T0
    sub \R0, [rip + prime_modulus +  0]
    sbb \R1, [rip + prime_modulus +  8]
    sbb \R2, [rip + prime_modulus + 16]
    sbb \R3, [rip + prime_modulus + 24]
    sbb \R4, [rip + prime_modulus + 32]
    sbb \R5, [rip + prime_modulus + 40]
    sbb \R6, [rip + prime_modulus + 48]
    sbb \R7, [rip + prime_modulus + 56]

	setnc al
	movzx \T0, al
	neg \T0

	SWAP2 \R0,   \M, \T0
	SWAP2 \R1,  8\M, \T0
	SWAP2 \R2, 16\M, \T0
	SWAP2 \R3, 24\M, \T0
	SWAP2 \R4, 32\M, \T0
	SWAP2 \R5, 40\M, \T0
	SWAP2 \R6, 48\M, \T0
	SWAP2 \R7, 56\M, \T0
.endm

.macro MULADD64x512 M1, Z0, Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8, T0, T1, C
	xor    \C, \C
    mulx   \T0, \T1, \M1     // A0*B0
    adox   \Z0, \T1
    adox   \Z1, \T0
    mulx   \T0, \T1, 8\M1    // A0*B1
    adcx   \Z1, \T1
    adox   \Z2, \T0
    mulx   \T0, \T1, 16\M1   // A0*B2
    adcx   \Z2, \T1
    adox   \Z3, \T0
    mulx   \T0, \T1, 24\M1   // A0*B3
    adcx   \Z3, \T1
    adox   \Z4, \T0
    mulx   \T0, \T1, 32\M1   // A0*B4
    adcx   \Z4, \T1
    adox   \Z5, \T0
    mulx   \T0, \T1, 40\M1   // A0*B5
    adcx   \Z5, \T1
    adox   \Z6, \T0
    mulx   \T0, \T1, 48\M1   // A0*B6
    adcx   \Z6, \T1
    adox   \Z7, \T0
    mulx   \T0, \T1, 56\M1   // A0*B7
    adcx   \Z7, \T1
    adox   \Z8, \T0
    adc    \Z8, 0
.endm


.macro MULADD64x320 M1, Z0, Z1, Z2, Z3, Z4, Z5, T0, T1
	xor    \T0, \T0
    mulx   \T0, \T1, \M1     // A0*B0
    adox   \Z0, \T1
    adox   \Z1, \T0
    mulx   \T0, \T1, 8\M1    // A0*B1
    adcx   \Z1, \T1
    adox   \Z2, \T0
    mulx   \T0, \T1, 16\M1   // A0*B2
    adcx   \Z2, \T1
    adox   \Z3, \T0
    mulx   \T0, \T1, 24\M1   // A0*B3
    adcx   \Z3, \T1
    adox   \Z4, \T0
    mulx   \T0, \T1, 32\M1   // A0*B4
    adcx   \Z4, \T1
    adox   \Z5, \T0
    adc    \Z5, 0
.endm

.macro FPMUL512x512 M0, M1, Z0, Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8, T0, T1
    // [Z1:Z7, Z8] <- z = (z0 x p503p1 + z)/2^64
    mov    rdx, \Z0                 // rdx <- z0
    MULADD64x320 [rip+prime_modulus_p1+24], \Z3, \Z4, \Z5, \Z6, \Z7, \Z8, \T0, \T1

    // [Z1:Z7, Z8, Z0] <- z = a01 x a1 + z
    mov    rdx, 8\M0
    MULADD64x512 \M1, \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \Z8, \Z0, \T0, \T1, \Z0
    // [Z2:Z7, Z8, Z0] <- z = (z0 x p503p1 + z)/2^64
    mov    rdx, \Z1                 // rdx <- z0
    MULADD64x320 [rip+prime_modulus_p1+24], \Z4, \Z5, \Z6, \Z7, \Z8, \Z0, \T0, \T1

    // [Z2:Z7, Z8, Z0:Z1] <- z = a02 x a1 + z
    mov    rdx, 16\M0
    MULADD64x512 \M1, \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \Z8, \Z0, \Z1, \T0, \T1, \Z1
    // [Z3:Z7, Z8, Z0:Z1] <- z = (z0 x p503p1 + z)/2^64
    mov    rdx, \Z2                // rdx <- z0
    MULADD64x320 [rip+prime_modulus_p1+24], \Z5, \Z6, \Z7, \Z8, \Z0, \Z1, \T0, \T1

    // [Z3:Z7, Z8, Z8, Z0:Z2] <- z = a03 x a1 + z
    mov    rdx, 24\M0
    MULADD64x512 \M1, \Z3, \Z4, \Z5, \Z6, \Z7, \Z8, \Z0, \Z1, \Z2, \T0, \T1, \Z2
    // [Z4:Z7, Z8, Z0:Z2] <- z = (z0 x p503p1 + z)/2^64
    mov    rdx, \Z3                // rdx <- z0
    MULADD64x320 [rip+prime_modulus_p1+24], \Z6, \Z7, \Z8, \Z0, \Z1, \Z2, \T0, \T1

    // [Z4:Z7, Z8, Z0:Z3] <- z = a04 x a1 + z
    mov    rdx, 32\M0
    MULADD64x512 \M1, \Z4, \Z5, \Z6, \Z7, \Z8, \Z0, \Z1, \Z2, \Z3, \T0, \T1, \Z3
    // [Z5:Z7, Z8, Z0:Z3] <- z = (z0 x p503p1 + z)/2^64
    mov    rdx, \Z4                // rdx <- z0
    MULADD64x320 [rip+prime_modulus_p1+24], \Z7, \Z8, \Z0, \Z1, \Z2, \Z3, \T0, \T1

    // [Z5:Z7, Z8, Z0:Z4] <- z = a05 x a1 + z
    mov    rdx, 40\M0
    MULADD64x512 \M1, \Z5, \Z6, \Z7, \Z8, \Z0, \Z1, \Z2, \Z3, \Z4, \T0, \T1, \Z4
    // [Z6:Z7, Z8, Z0:Z4] <- z = (z0 x p503p1 + z)/2^64
    mov    rdx, \Z5                // rdx <- z0
    MULADD64x320 [rip+prime_modulus_p1+24], \Z8, \Z0, \Z1, \Z2, \Z3, \Z4, \T0, \T1

    // [Z6:Z7, Z8, Z0:Z5] <- z = a06 x a1 + z
    mov    rdx, 48\M0
    MULADD64x512 \M1, \Z6, \Z7, \Z8, \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \T0, \T1, \Z5
    // [Z7, Z8, Z0:Z5] <- z = (z0 x p503p1 + z)/2^64
    mov    rdx, \Z6                // rdx <- z0
    MULADD64x320 [rip+prime_modulus_p1+24], \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \T0, \T1

    // [Z7, Z8, Z0:Z6] <- z = a07 x a1 + z
    mov    rdx, 56\M0
    MULADD64x512 \M1, \Z7, \Z8, \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \T0, \T1, \Z6
    // [Z8, Z0:Z6] <- z = (z0 x p503p1 + z)/2^64
    mov    rdx, \Z7                // rdx <- z0
    MULADD64x320 [rip+prime_modulus_p1+24], \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \T0, \T1
.endm


.global fmt(fp2sqr_re_asm)
fmt(fp2sqr_re_asm):
    push   r12
    push   r13

    // a0 + a1
    mov    r8, [reg_p1]
    mov    r9, [reg_p1+8]
    mov    r10, [reg_p1+16]
    mov    r11, [reg_p1+24]
    mov    r12, [reg_p1+32]
    mov    r13, [reg_p1+40]
    add    r8, [reg_p1+64]
    adc    r9, [reg_p1+72]
    push   r14
    adc    r10, [reg_p1+80]
    adc    r11, [reg_p1+88]
    push   r15
    adc    r12, [reg_p1+96]
    adc    r13, [reg_p1+104]
    mov    r14, [reg_p1+48]
    mov    r15, [reg_p1+56]
    adc    r14, [reg_p1+112]
    adc    r15, [reg_p1+120]
    mov    [reg_p2+8], r9
    mov    [reg_p2+16], r10
    mov    [reg_p2+24], r11
    mov    [reg_p2+32], r12
    mov    [reg_p2+40], r13
    mov    [reg_p2+48], r14
    mov    [reg_p2+56], r15

    // a0 - a1 + 4xp503
    mov    rcx, [reg_p1]
    mov    r10, [reg_p1+8]
    mov    r12, [reg_p1+16]
    mov    r13, [reg_p1+24]
    mov    r14, [reg_p1+32]
    mov    r15, [reg_p1+40]
    sub    rcx, [reg_p1+64]
    sbb    r10, [reg_p1+72]
    push   rbx
    sbb    r12, [reg_p1+80]
    sbb    r13, [reg_p1+88]
    push   rbp
    sbb    r14, [reg_p1+96]
    sbb    r15, [reg_p1+104]
    mov    rbx, [reg_p1+48]
    mov    rbp, [reg_p1+56]
    sbb    rbx, [reg_p1+112]
    sbb    rbp, [reg_p1+120]
    add    rcx, [rip+prime_modulus_x4]
    mov    rdx, [rip+prime_modulus_x4+8]
    adc    r10, rdx
    adc    r12, rdx
    adc    r13, [rip+prime_modulus_x4+24]
    adc    r14, [rip+prime_modulus_x4+32]
    adc    r15, [rip+prime_modulus_x4+40]
    adc    rbx, [rip+prime_modulus_x4+48]
    adc    rbp, [rip+prime_modulus_x4+56]
    mov    [reg_p2+64], rcx
    mov    [reg_p2+72], r10

    // [r8:r15, rbp] <- z = a00 x a1
    mov    rdx, r8
    mulx   r9, r8, rcx
    xor    rax, rax
    mov    [reg_p2+80], r12
    mulx   r10, r11, r10
    mov    [reg_p2+88], r13
    adox   r9, r11
    mulx   r11, r12, r12
    mov    [reg_p2+96], r14
    adox   r10, r12
    mulx   r12, r13, r13
    mov    [reg_p2+104], r15
    adox   r11, r13
    mulx   r13, r14, r14
    mov    [reg_p2+112], rbx
    adox   r12, r14
    mulx   r14, r15, r15
    mov    [reg_p2+120], rbp
    adox   r13, r15
    mulx   r15, rbp, rbx
    adox   r14, rbp
    mulx   rbp, rbx, [reg_p2+120]
    adox   r15, rbx
    adox   rbp, rax

    FPMUL512x512 [reg_p2], [reg_p2+64], r8, r9, r10, r11, r12, r13, r14, r15, rbp, rbx, rcx

    mov    [reg_p2], rbp
    mov    [reg_p2+8], r8
    mov    [reg_p2+16], r9
    mov    [reg_p2+24], r10
    mov    [reg_p2+32], r11
    mov    [reg_p2+40], r12
    mov    [reg_p2+48], r13
    mov    [reg_p2+56], r14

    REDUCE512 [reg_p2], rbp, r8, r9, r10, r11, r12, r13, r14, rbx

    pop    rbp
    pop    rbx
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    ret


.global fmt(fp2sqr_im_asm)
fmt(fp2sqr_im_asm):
    push   r12
    push   r13

    mov    r8, [reg_p1]
    mov    r9, [reg_p1+8]
    mov    r10, [reg_p1+16]
    mov    r11, [reg_p1+24]
    mov    r12, [reg_p1+32]
    mov    r13, [reg_p1+40]
    add    r8, r8
    adc    r9, r9
    push   r14
    adc    r10, r10
    adc    r11, r11
    push   r15
    adc    r12, r12
    mov    r14, [reg_p1+48]
    mov    r15, [reg_p1+56]
    adc    r13, r13
    push   rbx
    adc    r14, r14
    push   rbp
    adc    r15, r15
    sub    rsp, 64
    mov    [rsp+8], r9
    mov    [rsp+16], r10

    // [r8:r15, rbp] <- z = a00 x a1
    mov    rdx, r8
    mulx   r9, r8, [reg_p1+64]
    mov    [rsp+24], r11
    xor    rax, rax
    mulx   r10, r11, [reg_p1+72]
    mov    [rsp+32], r12
    adox   r9, r11
    mulx   r11, r12, [reg_p1+80]
    mov    [rsp+40], r13
    adox   r10, r12
    mulx   r12, r13, [reg_p1+88]
    mov    [rsp+48], r14
    adox   r11, r13
    mulx   r13, r14, [reg_p1+96]
    mov    [rsp+56], r15
    adox   r12, r14
    mulx   r14, r15, [reg_p1+104]
    adox   r13, r15
    mulx   r15, rbp, [reg_p1+112]
    adox   r14, rbp
    mulx   rbp, rbx, [reg_p1+120]
    adox   r15, rbx
    adox   rbp, rax

    FPMUL512x512 [rsp], [reg_p1+64], r8, r9, r10, r11, r12, r13, r14, r15, rbp, rbx, rcx

    mov    [reg_p2], rbp
    mov    [reg_p2+8], r8
    mov    [reg_p2+16], r9
    mov    [reg_p2+24], r10
    mov    [reg_p2+32], r11
    mov    [reg_p2+40], r12
    mov    [reg_p2+48], r13
    mov    [reg_p2+56], r14

    REDUCE512 [reg_p2], rbp, r8, r9, r10, r11, r12, r13, r14, rbx

    add    rsp, 64
    pop    rbp
    pop    rbx
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    ret


.global fmt(prime_field_multiplication)
fmt(prime_field_multiplication):
    mov    rcx, reg_p3
    push   rdx

    // [r8:r15] <- z = a x b0
    mov    rdx, [rcx + 0]
    mulx   r9,  r8, [reg_p2]
    push   r12
    xor    rax, rax
    mulx   r10, r11, [reg_p2+8]
    push   r13
    adox   r9,  r11
    mulx   r11, r12, [reg_p2+16]
    push   r14
    adox   r10, r12
    mulx   r12, r13, [reg_p2+24]
    push   r15
    adox   r11, r13
    mulx   r13, r14, [reg_p2+32]
    push   rbx
    adox   r12, r14
    mulx   r14, r15, [reg_p2+40]
    push   rbp
    adox   r13, r15
    mulx   r15, rbx, [reg_p2+48]
    adox   r14, rbx
    mulx   rbx, rbp, [reg_p2+56]
    adox   r15, rbp
    adox   rbx, rax

    FPMUL512x512 [rcx], [reg_p2], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rax, rbp

    mov    [reg_p1], rbx
    mov    [reg_p1+8], r8
    mov    [reg_p1+16], r9
    mov    [reg_p1+24], r10
    mov    [reg_p1+32], r11
    mov    [reg_p1+40], r12
    mov    [reg_p1+48], r13
    mov    [reg_p1+56], r14

    REDUCE512 [reg_p1], rbx, r8, r9, r10, r11, r12, r13, r14, rax

    pop    rbp
    pop    rbx
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    rdx
    ret


.global fmt(prime_field_square)
fmt(prime_field_square):
    mov rdx, rsi
    jmp fmt(prime_field_multiplication)

