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
    .quad 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
    .quad 0x0000000000000000, 0xc1ccf59098e20000, 0x91ca3591a0810f4f
    .quad 0xc3a747738cbaad7d, 0x3e568459654d5f6b, 0x000000030f5eba42

.global prime_modulus_x4
prime_modulus_x4:
    .quad 0xfffffffffffffffc, 0xffffffffffffffff, 0xffffffffffffffff
    .quad 0xffffffffffffffff, 0x0733d6426387ffff, 0x4728d64682043d3f
    .quad 0x0e9d1dce32eab5f6, 0xf95a116595357daf, 0x0000000c3d7ae908

.text
.P2ALIGN 4,,15

.macro MULADD64x576 M1, Z0, Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8, Z9, T0, T1, C
    mulx   \T0, \T1, \M1     // A0*B0
    xor    \C, \C
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
    mulx   \T0, \T1, 48\M1   // A0*B5
    adcx   \Z6, \T1
    adox   \Z7, \T0
    mulx   \T0, \T1, 56\M1   // A0*B5
    adcx   \Z7, \T1
    adox   \Z8, \T0
    mulx   \T0, \T1, 64\M1   // A0*B5
    adcx   \Z8, \T1
    adox   \Z9, \T0
    adc    \Z9, 0
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

.macro FPMUL576x576 M0, M1, Z0, Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8, Z9, T0, T1
    mov    rdx, \Z0                 // rdx <- z0
    MULADD64x320 [rip+prime_modulus_p1+32], \Z4, \Z5, \Z6, \Z7, \Z8, \Z9, \T0, \T1

    mov    rdx, 8\M0
    MULADD64x576 \M1, \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \Z8, \Z9, \Z0, \T0, \T1, \Z0
    mov    rdx, \Z1                 // rdx <- z1
    MULADD64x320 [rip+prime_modulus_p1+32], \Z5, \Z6, \Z7, \Z8, \Z9, \Z0, \T0, \T1

    mov    rdx, 16\M0
    MULADD64x576 \M1, \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \Z8, \Z9, \Z0, \Z1, \T0, \T1, \Z1
    mov    rdx, \Z2                // rdx <- z2
    MULADD64x320 [rip+prime_modulus_p1+32], \Z6, \Z7, \Z8, \Z9, \Z0, \Z1, \T0, \T1

    mov    rdx, 24\M0
    MULADD64x576 \M1, \Z3, \Z4, \Z5, \Z6, \Z7, \Z8, \Z9, \Z0, \Z1, \Z2, \T0, \T1, \Z2
    mov    rdx, \Z3                // rdx <- z3
    MULADD64x320 [rip+prime_modulus_p1+32], \Z7, \Z8, \Z9, \Z0, \Z1, \Z2, \T0, \T1

    mov    rdx, 32\M0
    MULADD64x576 \M1, \Z4, \Z5, \Z6, \Z7, \Z8, \Z9, \Z0, \Z1, \Z2, \Z3, \T0, \T1, \Z3
    mov    rdx, \Z4                // rdx <- z4
    MULADD64x320 [rip+prime_modulus_p1+32], \Z8, \Z9, \Z0, \Z1, \Z2, \Z3, \T0, \T1

    mov    rdx, 40\M0
    MULADD64x576 \M1, \Z5, \Z6, \Z7, \Z8, \Z9, \Z0, \Z1, \Z2, \Z3, \Z4, \T0, \T1, \Z4
    mov    rdx, \Z5                // rdx <- z5
    MULADD64x320 [rip+prime_modulus_p1+32], \Z9, \Z0, \Z1, \Z2, \Z3, \Z4, \T0, \T1

    mov    rdx, 48\M0
    MULADD64x576 \M1, \Z6, \Z7, \Z8, \Z9, \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \T0, \T1, \Z5
    mov    rdx, \Z6                // rdx <- z5
    MULADD64x320 [rip+prime_modulus_p1+32], \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \T0, \T1

    mov    rdx, 56\M0
    MULADD64x576 \M1, \Z7, \Z8, \Z9, \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \T0, \T1, \Z6
    mov    rdx, \Z7                // rdx <- z5
    MULADD64x320 [rip+prime_modulus_p1+32], \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \T0, \T1

    mov    rdx, 64\M0
    MULADD64x576 \M1, \Z8, \Z9, \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \T0, \T1, \Z7
    mov    rdx, \Z8                // rdx <- z5
    MULADD64x320 [rip+prime_modulus_p1+32], \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \T0, \T1
.endm


.macro SWAP2, R0, R1, T0
	xor \R0, \R1
	and \R0, \T0
	xor \R1, \R0
.endm

.macro REDUCE576 M, R0, R1, R2, R3, R4, R5, R6, R7, R8, T0
    sub \R0, [rip + prime_modulus +  0]
    sbb \R1, [rip + prime_modulus +  8]
    sbb \R2, [rip + prime_modulus + 16]
    sbb \R3, [rip + prime_modulus + 24]
    sbb \R4, [rip + prime_modulus + 32]
    sbb \R5, [rip + prime_modulus + 40]
    sbb \R6, [rip + prime_modulus + 48]
    sbb \R7, [rip + prime_modulus + 56]
    sbb \R8, [rip + prime_modulus + 64]

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
	SWAP2 \R8, 64\M, \T0
.endm

.global fmt(fp2sqr_re_asm)
fmt(fp2sqr_re_asm):
    push   r12
    push   r13
    push   r14
    push   r15
    push   rbx
    push   rbp

	// a0 + a1
	//load
	mov     r8, [reg_p1 +  0]
	mov     r9, [reg_p1 +  8]
	mov    r10, [reg_p1 + 16]
	mov    r11, [reg_p1 + 24]
	mov    r12, [reg_p1 + 32]
	mov    r13, [reg_p1 + 40]
	mov    r14, [reg_p1 + 48]
	mov    r15, [reg_p1 + 56]
	mov    rbx, [reg_p1 + 64]
	//add
	add     r8, [reg_p1 +  72]
	adc     r9, [reg_p1 +  80]
	adc    r10, [reg_p1 +  88]
	adc    r11, [reg_p1 +  96]
	adc    r12, [reg_p1 + 104]
	adc    r13, [reg_p1 + 112]
	adc    r14, [reg_p1 + 120]
	adc    r15, [reg_p1 + 128]
	adc    rbx, [reg_p1 + 136]
	//store
	mov    [reg_p2 +  0], r8
	mov    [reg_p2 +  8], r9
	mov    [reg_p2 + 16], r10
	mov    [reg_p2 + 24], r11
	mov    [reg_p2 + 32], r12
	mov    [reg_p2 + 40], r13
	mov    [reg_p2 + 48], r14
	mov    [reg_p2 + 56], r15
	mov    [reg_p2 + 64], rbx

	// a0 - a1 + 4xp434
	//load
 	mov    rax, [reg_p1 +  0]
	mov    r10, [reg_p1 +  8]
	mov    r11, [reg_p1 + 16]
	mov    r12, [reg_p1 + 24]
	mov    r13, [reg_p1 + 32]
	mov    r14, [reg_p1 + 40]
	mov    r15, [reg_p1 + 48]
	mov    rbx, [reg_p1 + 56]
	mov    rbp, [reg_p1 + 64]
	//sub
	sub    rax, [reg_p1 +  72]
	sbb    r10, [reg_p1 +  80]
	sbb    r11, [reg_p1 +  88]
	sbb    r12, [reg_p1 +  96]
	sbb    r13, [reg_p1 + 104]
	sbb    r14, [reg_p1 + 112]
	sbb    r15, [reg_p1 + 120]
	sbb    rbx, [reg_p1 + 128]
	sbb    rbp, [reg_p1 + 136]
	//add
 	add    rax, [rip + prime_modulus_x4 +  0]
	adc    r10, [rip + prime_modulus_x4 +  8]
	adc    r11, [rip + prime_modulus_x4 + 16]
	adc    r12, [rip + prime_modulus_x4 + 24]
	adc    r13, [rip + prime_modulus_x4 + 32]
	adc    r14, [rip + prime_modulus_x4 + 40]
	adc    r15, [rip + prime_modulus_x4 + 48]
	adc    rbx, [rip + prime_modulus_x4 + 56]
	adc    rbp, [rip + prime_modulus_x4 + 64]
    //store
	mov    [reg_p2 +  72], rax
	mov    [reg_p2 +  80], r10
	mov    [reg_p2 +  88], r11
	mov    [reg_p2 +  96], r12
	mov    [reg_p2 + 104], r13
	mov    [reg_p2 + 112], r14
	mov    [reg_p2 + 120], r15
	mov    [reg_p2 + 128], rbx
	mov    [reg_p2 + 136], rbp

    // [r8:r14] <- z = a00 x a1
    mov    rdx, r8
    mulx   r9, r8, rax
    xor    rax, rax //zero
    mulx   r10, rcx, r10
    adox   r9,  rcx
    mulx   r11, rcx, r11
    adox   r10, rcx
    mulx   r12, rcx, r12
    adox   r11, rcx
    mulx   r13, rcx, r13
    adox   r12, rcx
    mulx   r14, rcx, r14
    adox   r13, rcx
    mulx   r15, rcx, r15
    adox   r14, rcx
    mulx   rbx, rcx, rbx
    adox   r15, rcx
    mulx   rbp, rcx, rbp
    adox   rbx, rcx
    adox   rbp, rax

	FPMUL576x576 [reg_p2], [reg_p2 + 72], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rbp, rax, rcx

    mov    [reg_p2], rbp
    mov    [reg_p2+8], r8
    mov    [reg_p2+16], r9
    mov    [reg_p2+24], r10
    mov    [reg_p2+32], r11
    mov    [reg_p2+40], r12
    mov    [reg_p2+48], r13
    mov    [reg_p2+56], r14
    mov    [reg_p2+64], r15

    REDUCE576 [reg_p2], rbp, r8, r9, r10, r11, r12, r13, r14, r15, rbx

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
    push   r14
    push   r15
    push   rbx
    push   rbp

    //load
	mov    r8,  [reg_p1]
 	mov    r9,  [reg_p1 + 8]
	mov    r10, [reg_p1 + 16]
	mov    r11, [reg_p1 + 24]
	mov    r12, [reg_p1 + 32]
	mov    r13, [reg_p1 + 40]
	mov    r14, [reg_p1 + 48]
	mov    r15, [reg_p1 + 56]
	mov    rbx, [reg_p1 + 64]

    //2*a0
	add    r8,  r8
	adc    r9,  r9
	adc    r10, r10
	adc    r11, r11
	adc    r12, r12
	adc    r13, r13
	adc    r14, r14
	adc    r15, r15
	adc    rbx, rbx

	//store
	sub    rsp, 72
	mov    [rsp +  8], r9
	mov    [rsp + 16], r10
	mov    [rsp + 24], r11
	mov    [rsp + 32], r12
	mov    [rsp + 40], r13
	mov    [rsp + 48], r14
	mov    [rsp + 56], r15
	mov    [rsp + 64], rbx

    // [r8:r14] <- z = a00 x a1
    mov    rdx, r8
    mulx   r9,  r8,  [reg_p1 +  72]
    xor    rax, rax
    mulx   r10, r11, [reg_p1 +  80]
    adox   r9,  r11
    mulx   r11, r12, [reg_p1 +  88]
    adox   r10, r12
    mulx   r12, r13, [reg_p1 +  96]
    adox   r11, r13
    mulx   r13, r14, [reg_p1 + 104]
    adox   r12, r14
    mulx   r14, r15, [reg_p1 + 112]
    adox   r13, r15
    mulx   r15, rbx, [reg_p1 + 120]
    adox   r14, rbx
    mulx   rbx, rbp, [reg_p1 + 128]
    adox   r15, rbp
    mulx   rbp, rcx, [reg_p1 + 136]
    adox   rbx, rcx
    adox   rbp, rax

	FPMUL576x576 [rsp], [reg_p1 + 72], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rbp, rax, rcx

	add    rsp, 72
    mov    [reg_p2], rbp
    mov    [reg_p2+8], r8
    mov    [reg_p2+16], r9
    mov    [reg_p2+24], r10
    mov    [reg_p2+32], r11
    mov    [reg_p2+40], r12
    mov    [reg_p2+48], r13
    mov    [reg_p2+56], r14
    mov    [reg_p2+64], r15

    REDUCE576 [reg_p2], rbp, r8, r9, r10, r11, r12, r13, r14, r15, rax

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
    push   r12
    push   r13
    push   r14
    push   r15
    push   rbx
    push   rbp
    push   reg_p1

    // [r8:r14] <- z = a x b0
    mov    rdx, [rcx + 0]
    mulx   r9, r8, [reg_p2]
    xor    rax, rax
    mulx   r10, r11, [reg_p2+8]
    adox   r9, r11
    mulx   r11, r12, [reg_p2+16]
    adox   r10, r12
    mulx   r12, r13, [reg_p2+24]
    adox   r11, r13
    mulx   r13, r14, [reg_p2+32]
    adox   r12, r14
    mulx   r14, r15, [reg_p2+40]
    adox   r13, r15
    mulx   r15, rbx, [reg_p2+48]
    adox   r14, rbx
    mulx   rbx, rbp, [reg_p2+56]
    adox   r15, rbp
    mulx   rbp, reg_p1, [reg_p2+64]
    adox   rbx, reg_p1
    adox   rbp, rax

	FPMUL576x576 [rcx], [reg_p2], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rbp, rax, reg_p1

    pop    reg_p1

    mov    [reg_p1], rbp
    mov    [reg_p1+8], r8
    mov    [reg_p1+16], r9
    mov    [reg_p1+24], r10
    mov    [reg_p1+32], r11
    mov    [reg_p1+40], r12
    mov    [reg_p1+48], r13
    mov    [reg_p1+56], r14
    mov    [reg_p1+64], r15


    REDUCE576 [reg_p1], rbp, r8, r9, r10, r11, r12, r13, r14, r15, rax

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
