.intel_syntax noprefix

// format function and variable names for mac os x
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
    .quad 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
    .quad 0x0000000000000000, 0x604054B000000000, 0xDF4970CF7313736F, 0x719AEC973BF54225
    .quad 0x40E474DA88B90FFE, 0x9A0E279D6CEB3C8E, 0x01B39F97671708CF


.global prime_modulus_x4
prime_modulus_x4:
    .quad 0xFFFFFFFFFFFFFFFC, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
    .quad 0xFFFFFFFFFFFFFFFF, 0x810152BFFFFFFFFF, 0x7D25C33DCC4DCDBD, 0xC66BB25CEFD50897
    .quad 0x0391D36A22E43FF9, 0x68389E75B3ACF239, 0x06CE7E5D9C5C233E

.text
.p2align 4,,15


.macro muladd64x704b m1, m, mm, z4, z5, z6, z7, z8, z9, z10, z11, t0, t1, t2, c
    mulx   \t0, \t1, \m1     // a0*b0
	xor    \c, \c
    adox   \t1, \m
    adox   \t0, 8\m
	mov    32\m, \t1
    mulx   \t1, \t2, 8\m1    // a0*b1
    adcx   \t0, \t2
    adox   \t1, 16\m
	mov    \mm, \t0
    mulx   \t0, \t2, 16\m1   // a0*b2
    adcx   \t1, \t2
    adox   \t0, 24\m
	mov    8\mm, \t1
    mulx   \t1, \t2, 24\m1   // a0*b3
    adcx   \t0, \t2
    adox   \z4, \t1
	mov    16\mm, \t0
    mulx   \t0, \t1, 32\m1   // a0*b4
    adcx   \z4, \t1
    adox   \z5, \t0
    mulx   \t0, \t1, 40\m1   // a0*b5
	mov    24\mm, \z4
    adcx   \z5, \t1
    adox   \z6, \t0
    mulx   \t0, \t1, 48\m1   // a0*b6
    adcx   \z6, \t1
    adox   \z7, \t0
    mulx   \t0, \t1, 56\m1   // a0*b7
    adcx   \z7, \t1
    adox   \z8, \t0
    mulx   \t0, \t1, 64\m1   // a0*b8
    adcx   \z8, \t1
    adox   \z9, \t0
    mulx   \t0, \t1, 72\m1   // a0*b9
    adcx   \z9, \t1
    adox   \z10, \t0
    mulx   \t0, \t1, 80\m1   // a0*b9
    adcx   \z10, \t1
    adox   \z11, \t0
    adc    \z11, 0
.endm


.macro muladd64x384 m1, z0, z1, z2, z3, z4, z5, z6, t0, t1
    mulx   \t0, \t1, \m1     // a0*b0
	xor    rax, rax
    adox   \z0, \t1
    adox   \z1, \t0
    mulx   \t0, \t1, 8\m1    // a0*b1
    adcx   \z1, \t1
    adox   \z2, \t0
    mulx   \t0, \t1, 16\m1   // a0*b2
    adcx   \z2, \t1
    adox   \z3, \t0
    mulx   \t0, \t1, 24\m1   // a0*b3
    adcx   \z3, \t1
    adox   \z4, \t0
    mulx   \t0, \t1, 32\m1   // a0*b4
    adcx   \z4, \t1
    adox   \z5, \t0
    mulx   \t0, \t1, 40\m1   // a0*b5
    adcx   \z5, \t1
    adox   \z6, \t0
    adcx   \z6, rax
.endm

.macro fpmul704x704 m0, m1, mm, out, z0, z1, z2, z3, z4, z5, z6, z7, t0, t1, t2
    mov    rdx, 32\mm                  // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z5, \z6, \z7, \z0, \z1, \z2, \z3, \t0, \t1

    mov    rdx, 8\m0
    muladd64x704b \m1, \mm, \mm, \z5, \z6, \z7, \z0, \z1, \z2, \z3, \z4, \t0, \t1, \t2, \z4
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z6, \z7, \z0, \z1, \z2, \z3, \z4, \t0, \t1

    mov    rdx, 16\m0
    muladd64x704b \m1, \mm, \mm, \z6, \z7, \z0, \z1, \z2, \z3, \z4, \z5, \t0, \t1, \t2, \z5
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z7, \z0, \z1, \z2, \z3, \z4, \z5, \t0, \t1

    mov    rdx, 24\m0
    muladd64x704b \m1, \mm, \mm, \z7, \z0, \z1, \z2, \z3, \z4, \z5, \z6, \t0, \t1, \t2, \z6
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z0, \z1, \z2, \z3, \z4, \z5, \z6, \t0, \t1

    mov    rdx, 32\m0
    muladd64x704b \m1, \mm, \mm, \z0, \z1, \z2, \z3, \z4, \z5, \z6, \z7, \t0, \t1, \t2, \z7
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z1, \z2, \z3, \z4, \z5, \z6, \z7, \t0, \t1

    mov    rdx, 40\m0
    muladd64x704b \m1, \mm, \mm, \z1, \z2, \z3, \z4, \z5, \z6, \z7, \z0, \t0, \t1, \t2, \z0
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z2, \z3, \z4, \z5, \z6, \z7, \z0, \t0, \t1

    mov    rdx, 48\m0
    muladd64x704b \m1, \mm, \mm, \z2, \z3, \z4, \z5, \z6, \z7, \z0, \z1, \t0, \t1, \t2, \z1
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z3, \z4, \z5, \z6, \z7, \z0, \z1, \t0, \t1

    mov    rdx, 56\m0
    muladd64x704b \m1, \mm, \mm, \z3, \z4, \z5, \z6, \z7, \z0, \z1, \z2, \t0, \t1, \t2, \z2
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z4, \z5, \z6, \z7, \z0, \z1, \z2, \t0, \t1

    mov    rdx, 64\m0
    muladd64x704b \m1, \mm, \mm, \z4, \z5, \z6, \z7, \z0, \z1, \z2, \z3, \t0, \t1, \t2, \z3
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z5, \z6, \z7, \z0, \z1, \z2, \z3, \t0, \t1

    mov    rdx, 72\m0
    muladd64x704b \m1, \mm, \mm, \z5, \z6, \z7, \z0, \z1, \z2, \z3, \z4 \t0, \t1, \t2, \z4
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z6, \z7, \z0, \z1, \z2, \z3, \z4, \t0, \t1

    mov    rdx, 80\m0
    muladd64x704b \m1, \mm, \out, \z6, \z7, \z0, \z1, \z2, \z3, \z4, \z5, \t0, \t1, \t2, \z5
    mov    rdx, 32\mm                 // rdx <- z0
    muladd64x384 [rip+prime_modulus_p1+40], \z7, \z0, \z1, \z2, \z3, \z4, \z5, \t0, \t1
.endm

.macro SWAP2, R0, R1, T0
	xor \R0, \R1
	and \R0, \T0
	xor \R1, \R0
.endm

.macro REDUCE704 M, R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, T0
    sub \R0,  [rip + prime_modulus +  0]
    sbb \R1,  [rip + prime_modulus +  8]
    sbb \R2,  [rip + prime_modulus + 16]
    sbb \R3,  [rip + prime_modulus + 24]
    sbb \R4,  [rip + prime_modulus + 32]
    sbb \R5,  [rip + prime_modulus + 40]
    sbb \R6,  [rip + prime_modulus + 48]
    sbb \R7,  [rip + prime_modulus + 56]
    sbb \R8,  [rip + prime_modulus + 64]
    sbb \R9,  [rip + prime_modulus + 72]
    sbb \R10, [rip + prime_modulus + 80]

	setnc al
	movzx \T0, al
	neg \T0

	SWAP2 \R0,    \M, \T0
	SWAP2 \R1,   8\M, \T0
	SWAP2 \R2,  16\M, \T0
	SWAP2 \R3,  24\M, \T0
	SWAP2 \R4,  32\M, \T0
	SWAP2 \R5,  40\M, \T0
	SWAP2 \R6,  48\M, \T0
	SWAP2 \R7,  56\M, \T0
	SWAP2 \R8,  64\M, \T0
	SWAP2 \R9,  72\M, \T0
	SWAP2 \R10, 80\M, \T0
.endm


.global fmt(fp2sqr_re_asm)
fmt(fp2sqr_re_asm):
    push   r12
    push   r13
    push   r14
    push   r15
    push   rbx
    push   rbp
	sub    rsp, 40

	// a0 + a1
	mov    r8,  [reg_p1]
	mov    r9,  [reg_p1+8]
	mov    r10, [reg_p1+16]
	mov    r11, [reg_p1+24]
	mov    r12, [reg_p1+32]
	mov    r13, [reg_p1+40]
	add    r8,  [reg_p1+88]
	adc    r9,  [reg_p1+96]
	mov    [reg_p2], r8
	adc    r10, [reg_p1+104]
	adc    r11, [reg_p1+112]
	mov    [reg_p2+8], r9
	mov    [reg_p2+16], r10
	mov    [reg_p2+24], r11
	adc    r12, [reg_p1+120]
	adc    r13, [reg_p1+128]
	mov    r14, [reg_p1+48]
	mov    r15, [reg_p1+56]
	adc    r14, [reg_p1+136]
	adc    r15, [reg_p1+144]
	mov    r9,  [reg_p1+64]
	mov    r10, [reg_p1+72]
	mov    r11, [reg_p1+80]
	adc    r9,  [reg_p1+152]
	adc    r10, [reg_p1+160]
	adc    r11, [reg_p1+168]
	mov    [reg_p2+32], r12
	mov    [reg_p2+40], r13
	mov    [reg_p2+48], r14
	mov    [reg_p2+56], r15
	mov    [reg_p2+64], r9
	mov    [reg_p2+72], r10
	mov    [reg_p2+80], r11

	// a0 - a1 + 4xp610
	mov    rcx, [reg_p1+0]
	mov    r10, [reg_p1+8]
	mov    r12, [reg_p1+16]
	mov    r13, [reg_p1+24]
	mov    r14, [reg_p1+32]
	mov    r15, [reg_p1+40]

	sub    rcx, [reg_p1+88]
	sbb    r10, [reg_p1+96]
	sbb    r12, [reg_p1+104]
	sbb    r13, [reg_p1+112]
	sbb    r14, [reg_p1+120]
	sbb    r15, [reg_p1+128]

	mov    rbx, [reg_p1+48]
	mov    rbp, [reg_p1+56]
	mov    r8,  [reg_p1+64]
	mov    r9,  [reg_p1+72]
	mov    rax, [reg_p1+80]

	sbb    rbx, [reg_p1+136]
	sbb    rbp, [reg_p1+144]
	sbb    r8,  [reg_p1+152]
	sbb    r9,  [reg_p1+160]
	sbb    rax, [reg_p1+168]

	add    rcx, [rip+prime_modulus_x4]
	mov    rdx, [rip+prime_modulus_x4+8]
	adc    r10, rdx
	adc    r12, rdx
	adc    r13, rdx
	adc    r14, rdx
	adc    r15, [rip+prime_modulus_x4+40]
	adc    rbx, [rip+prime_modulus_x4+48]
	adc    rbp, [rip+prime_modulus_x4+56]
	adc    r8,  [rip+prime_modulus_x4+64]
	adc    r9,  [rip+prime_modulus_x4+72]
	adc    rax, [rip+prime_modulus_x4+80]

	mov    [reg_p2+88], rcx
	mov    [reg_p2+96], r10
	mov    [reg_p2+104], r12
	mov    [reg_p2+112], r13
	mov    [reg_p2+120], r14
	mov    [reg_p2+152], r8
	mov    [reg_p2+160], r9
	mov    [reg_p2+168], rax

    // [rsp0:rsp16, r11:r15, r8:r10] <- z = a00 x a1
    mov    rdx, [reg_p2]
    mulx   r9, r8, rcx
	mov    [reg_p2+128], r15
    xor    rax, rax
    mulx   r10, r11, r10
	mov    [reg_p2+136], rbx
    adcx   r9, r11
    mulx   r11, r12, r12
	mov    [reg_p2+144], rbp
    adcx   r10, r12
    mulx   r12, r13, r13
	mov    [rsp+32], r8
    adcx   r11, r13
    mulx   r13, r8, r14
	mov    [rsp], r9
    adcx   r12, r8
    mulx   r14, r9, r15
	mov    [rsp+8], r10
    adcx   r13, r9
    mulx   r15, rax, rbx
	mov    [rsp+16], r11
    adcx   r14, rax
    mulx   r8, r10, rbp
	mov    [rsp+24], r12
    adcx   r15, r10
    mulx   r9, rax, [reg_p2+152]
    adcx   r8, rax
    mulx   r10, rbx, [reg_p2+160]
    adcx   r9, rbx
    mulx   r11, rax, [reg_p2+168]
    adcx   r10, rax
    adc    r11, 0

	fpmul704x704 [reg_p2], [reg_p2+88], [rsp], [reg_p2], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rbp, rax

    mov    [reg_p2+32], r15
    mov    [reg_p2+40], r8
    mov    [reg_p2+48], r9
    mov    [reg_p2+56], r10
    mov    [reg_p2+64], r11
    mov    [reg_p2+72], r12
    mov    [reg_p2+80], r13

    mov    rbp, [reg_p2 +   0]
    mov    rbx, [reg_p2 +   8]
    mov    rcx, [reg_p2 +  16]
    mov    r14, [reg_p2 +  24]

    REDUCE704 [reg_p2], rbp, rbx, rcx, r14, r15, r8, r9, r10, r11, r12, r13, rax

	add    rsp, 40
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

	mov    r8, [reg_p1]
	mov    r9, [reg_p1+8]
	mov    r10, [reg_p1+16]
	mov    r11, [reg_p1+24]
	mov    r12, [reg_p1+32]
	mov    r13, [reg_p1+40]
	add    r8, r8
	adc    r9, r9
    push   rbx
	adc    r10, r10
	adc    r11, r11
    push   rbp
	adc    r12, r12
	adc    r13, r13
	mov    r14, [reg_p1+48]
	mov    r15, [reg_p1+56]
	adc    r14, r14
	adc    r15, r15
	mov    rbx, [reg_p1+64]
	mov    rbp, [reg_p1+72]
	mov    rcx, [reg_p1+80]
	adc    rbx, rbx
	adc    rbp, rbp
	adc    rcx, rcx
	sub    rsp, 128 // sub 16 ?
	mov    [rsp+8], r9
	mov    [rsp+16], r10
	mov    [rsp+24], r11

    // [rsp24, rsp0:rsp16, r11:r15, r8:r10] <- z = a00 x a1
    mov    rdx, r8
    mulx   r9, r8, [reg_p1 + 88]
	mov    [rsp+32], r12
    xor    rax, rax
    mulx   r10, r11, [reg_p1 + 96]
	mov    [rsp+40], r13
    adcx   r9, r11
    mulx   r11, r12, [reg_p1 + 104]
	mov    [rsp+48], r14
    adcx   r10, r12
    mulx   r12, r13, [reg_p1 + 112]
	mov    [rsp+120], r8
    adcx   r11, r13
    mulx   r13, r8, [reg_p1 + 120]
	mov    [rsp+88], r9
    adcx   r12, r8
    mulx   r14, r9, [reg_p1 + 128]
	mov    [rsp+56], r15
    adcx   r13, r9
    mulx   r15, rax, [reg_p1 + 136]
	mov    [rsp+96], r10
    adcx   r14, rax
    mulx   r8, r10, [reg_p1 + 144]
	mov    [rsp+104], r11
    adcx   r15, r10
    mulx   r9, rax, [reg_p1 + 152]
	mov    [rsp+64], rbx
    adcx   r8, rax
    mulx   r10, rbx, [reg_p1 + 160]
	mov    [rsp+72], rbp
    adcx   r9, rbx
    mulx   r11, rax, [reg_p1 + 168]
	mov    [rsp+80], rcx
    adcx   r10, rax
	mov    [rsp+112], r12
    adc    r11, 0

	fpmul704x704 [rsp], [reg_p1+88], [rsp+88], [reg_p2], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rbp, rax

    mov    [reg_p2+32], r15
    mov    [reg_p2+40], r8
    mov    [reg_p2+48], r9
    mov    [reg_p2+56], r10
    mov    [reg_p2+64], r11
    mov    [reg_p2+72], r12
    mov    [reg_p2+80], r13

    mov    rbp, [reg_p2 +   0]
    mov    rbx, [reg_p2 +   8]
    mov    rcx, [reg_p2 +  16]
    mov    r14, [reg_p2 +  24]

    REDUCE704 [reg_p2], rbp, rbx, rcx, r14, r15, r8, r9, r10, r11, r12, r13, rax

	add    rsp, 128
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
	sub    rsp, 40

    // [r8:r15] <- z = a x b0
    mov    rdx, [reg_p3 + 0]
    mulx   r9, r8, [reg_p2]
    xor    rax, rax
	mov    [rsp+32], r8
    mulx   r10, r11, [reg_p2+8]
    adcx   r9, r11
    mulx   r11, r12, [reg_p2+16]
    adcx   r10, r12
    mulx   r12, r13, [reg_p2+24]
    adcx   r11, r13
    mulx   r13, r8, [reg_p2+32]
    adcx   r12, r8
    mulx   r14, rax, [reg_p2+40]
    adcx   r13, rax
    mulx   r15, rax, [reg_p2+48]
	mov    [rsp], r9
    adcx   r14, rax
    mulx   r8, rbx, [reg_p2+56]
	mov    [rsp+8], r10
    adcx   r15, rbx
    mulx   r9, rax, [reg_p2+64]
	mov    [rsp+16], r11
    adcx   r8, rax
    mulx   r10, rbx, [reg_p2+72]
	mov    [rsp+24], r12
    adcx   r9, rbx
    mulx   r11, rax, [reg_p2+80]
    adcx   r10, rax
    adc    r11, 0

	fpmul704x704 [rcx], [reg_p2], [rsp], [reg_p1], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rbp, rax

    mov    [reg_p1 + 32], r15
    mov    [reg_p1 + 40], r8
    mov    [reg_p1 + 48], r9
    mov    [reg_p1 + 56], r10
    mov    [reg_p1 + 64], r11
    mov    [reg_p1 + 72], r12
    mov    [reg_p1 + 80], r13

    mov    rbp, [reg_p1 +   0]
    mov    rbx, [reg_p1 +   8]
    mov    rcx, [reg_p1 +  16]
    mov    r14, [reg_p1 +  24]

    REDUCE704 [reg_p1], rbp, rbx, rcx, r14, r15, r8, r9, r10, r11, r12, r13, rax

	add    rsp, 40
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
