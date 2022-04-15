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
    .quad 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x6E02000000000000
    .quad 0xB1784DE8AA5AB02E, 0x9AE7BF45048FF9AB, 0xB255B2FA10C4252A, 0x819010C251E7D88C, 0x000000027BF6A768


.global prime_modulus_x4
prime_modulus_x4:
    .quad 0xFFFFFFFFFFFFFFFC, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xB807FFFFFFFFFFFF
    .quad 0xC5E137A2A96AC0B9, 0x6B9EFD14123FE6AE, 0xC956CBE8431094AA, 0x06404309479F6232, 0x00000009EFDA9DA2

.text
.P2ALIGN 4,,15


.reduce_once:
	push rbp
	push r12
	push r13
	mov rbp, rdi

	mov rdi, [rbp +  0]
	sub rdi, [rip + prime_modulus +  0]
	mov rsi, [rbp +  8]
	sbb rsi, [rip + prime_modulus + 8]
	mov rdx, [rbp +  16]
	sbb rdx, [rip + prime_modulus + 16]
	mov rcx, [rbp +  24]
	sbb rcx, [rip + prime_modulus + 24]
	mov r8, [rbp +  32]
	sbb r8, [rip + prime_modulus + 32]
	mov r9, [rbp +  40]
	sbb r9, [rip + prime_modulus + 40]
	mov r10, [rbp +  48]
	sbb r10, [rip + prime_modulus + 48]
	mov r11, [rbp +  56]
	sbb r11, [rip + prime_modulus + 56]

	mov r12, [rbp +  64]
	sbb r12, [rip + prime_modulus + 64]
	mov r13, [rbp +  72]
	sbb r13, [rip + prime_modulus + 72]

	setnc al
	movzx rax, al
	neg rax

.macro cswap2, r, m
	xor \r, \m
	and \r, rax
	xor \m, \r
.endm

	cswap2 rdi, [rbp +  0]
	cswap2 rsi, [rbp +  8]
	cswap2 rdx, [rbp +  16]
	cswap2 rcx, [rbp +  24]
	cswap2 r8, [rbp +  32]
	cswap2 r9, [rbp +  40]
	cswap2 r10, [rbp +  48]
	cswap2 r11, [rbp +  56]

	cswap2 r12, [rbp +  64]
	cswap2 r13, [rbp +  72]

	pop r13
	pop r12
	pop rbp
	ret



.macro MULADD64x640b M1, M, MM, Z3, Z4, Z5, Z6, Z7, Z8, Z9, Z10, T0, T1, T2, C
    mulx   \T0, \T1, \M1     // A0*B0
	xor    \C, \C
    adox   \T1, \M
    adox   \T0, 8\M
	mov    24\M, \T1
    mulx   \T1, \T2, 8\M1    // A0*B1
    adcx   \T0, \T2
    adox   \T1, 16\M
	mov    \MM, \T0
    mulx   \T0, \T2, 16\M1   // A0*B2
    adcx   \T1, \T2
    adox   \Z3, \T0
	mov    8\MM, \T1
    mulx   \T0, \T1, 24\M1   // A0*B3
    adcx   \Z3, \T1
    adox   \Z4, \T0
    mulx   \T0, \T1, 32\M1   // A0*B4
	mov    16\MM, \Z3
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
    mulx   \T0, \T1, 64\M1   // A0*B8
    adcx   \Z8, \T1
    adox   \Z9, \T0
    mulx   \T0, \T1, 72\M1   // A0*B9
    adcx   \Z9, \T1
    adox   \Z10, \T0
    adc    \Z10, 0
.endm


.macro MULADD64x384 M1, Z0, Z1, Z2, Z3, Z4, Z5, Z6, T0, T1
    mulx   \T0, \T1, \M1     // A0*B0
	xor    rax, rax
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
    adc    \Z6, 0
.endm

.macro FPMUL640x640 M0, M1, MM, OUT, Z0, Z1, Z2, Z3, Z4, Z5, Z6, Z7, T0, T1, T2
    // [Z4:Z7, Z0:Z2] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                  // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z4, \Z5, \Z6, \Z7, \Z0, \Z1, \Z2, \T0, \T1

    // [rsp8:rsp24, \Z4:\Z7, \Z0:\Z3] <- z = a0 x b11 + a1 x b01 + z
    mov    rdx, 8\M0
    MULADD64x640b \M1, \MM, \MM, \Z4, \Z5, \Z6, \Z7, \Z0, \Z1, \Z2, \Z3, \T0, \T1, \T2, \Z3
    // [\Z5:\Z7, \Z0:\Z3] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                 // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z5, \Z6, \Z7, \Z0, \Z1, \Z2, \Z3, \T0, \T1

    // [rsp16:rsp32, \Z5:\Z7, \Z0:\Z4] <- z = a0 x b12 + a1 x b02 + z
    mov    rdx, 16\M0
    MULADD64x640b \M1, \MM, \MM, \Z5, \Z6, \Z7, \Z0, \Z1, \Z2, \Z3, \Z4, \T0, \T1, \T2, \Z4
    // [rsp24:rsp40, \Z6:\Z7, \Z0:\Z4] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                 // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z6, \Z7, \Z0, \Z1, \Z2, \Z3, \Z4, \T0, \T1

    // [rsp24:rsp40, \Z6:\Z7, \Z0:\Z5] <- z = a0 x b13 + a1 x b03 + z
    mov    rdx, 24\M0
    MULADD64x640b \M1, \MM, \MM, \Z6, \Z7, \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \T0, \T1, \T2, \Z5
    // [rsp32:rsp48, \Z7, \Z0:\Z5] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                 // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z7, \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \T0, \T1

    // [rsp32:rsp48, \Z7, \Z0:\Z6] <- z = a0 x b14 + a1 x b04 + z
    mov    rdx, 32\M0
    MULADD64x640b \M1, \MM, \MM, \Z7, \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \T0, \T1, \T2, \Z6
    // [rsp40:rsp56, \Z0:\Z6] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                 // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \T0, \T1

    // [rsp40:rsp56, \Z0:\Z7] <- z = a0 x b15 + a1 x b05 + z
    mov    rdx, 40\M0
    MULADD64x640b \M1, \MM, \MM, \Z0, \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \T0, \T1, \T2, \Z7
    // [rsp48:rsp64, \Z1:\Z7] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                 // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \T0, \T1

    // [rsp48:rsp64, \Z1:\Z7, \Z0] <- z = a0 x b16 + a1 x b06 + z
    mov    rdx, 48\M0
    MULADD64x640b \M1, \MM, \MM, \Z1, \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \Z0, \T0, \T1, \T2, \Z0
    // [rsp56:rsp72, \Z2:\Z7, \Z0] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                 // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \Z0, \T0, \T1

    // [rsp56:rsp72, \Z2:\Z7, \Z0:\Z1] <- z = a0 x b17 + a1 x b07 + z
    mov    rdx, 56\M0
    MULADD64x640b \M1, \MM, \MM, \Z2, \Z3, \Z4, \Z5, \Z6, \Z7, \Z0, \Z1, \T0, \T1, \T2, \Z1
    // [rsp64:rsp80, \Z3:\Z7, \Z0:\Z1] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                 // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z3, \Z4, \Z5, \Z6, \Z7, \Z0, \Z1, \T0, \T1

    // [rsp64:rsp80, \Z3:\Z7, \Z0:\Z2] <- z = a0 x b18 + a1 x b08 + z
    mov    rdx, 64\M0
    MULADD64x640b \M1, \MM, \MM, \Z3, \Z4, \Z5, \Z6, \Z7, \Z0, \Z1, \Z2, \T0, \T1, \T2, \Z2
    // [rsp72:rsp88, \Z4:\Z7, \Z0:\Z2] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                 // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z4, \Z5, \Z6, \Z7, \Z0, \Z1, \Z2, \T0, \T1

    // [rsi0:rsi16, \Z4:\Z7, \Z0:\Z3] <- z = a0 x b19 + a1 x b09 + z
    mov    rdx, 72\M0
    MULADD64x640b \M1, \MM, \OUT, \Z4, \Z5, \Z6, \Z7, \Z0, \Z1, \Z2, \Z3, \T0, \T1, \T2, \Z3
    // [rsp80:rsp96, \Z5:\Z7, \Z0:\Z3] <- z = (z0 x p610p1 + z)/2^64
    mov    rdx, 24\MM                 // rdx <- z0
    MULADD64x384 [rip+prime_modulus_p1+32], \Z5, \Z6, \Z7, \Z0, \Z1, \Z2, \Z3, \T0, \T1
.endm



.global fmt(fp2sqr_re_asm)
fmt(fp2sqr_re_asm):
    push   r12
    push   r13
    push   r14
    push   r15
    push   rbx
    push   rbp
	sub    rsp, 32

	// a0 + a1
	mov    r8, [reg_p1]
	mov    r9, [reg_p1+8]
	mov    r10, [reg_p1+16]
	mov    r11, [reg_p1+24]
	mov    r12, [reg_p1+32]
	mov    r13, [reg_p1+40]
	add    r8, [reg_p1+80]
	adc    r9, [reg_p1+88]
	mov    [reg_p2], r8
	adc    r10, [reg_p1+96]
	adc    r11, [reg_p1+104]
	mov    [reg_p2+8], r9
	mov    [reg_p2+16], r10
	adc    r12, [reg_p1+112]
	adc    r13, [reg_p1+120]
	mov    r14, [reg_p1+48]
	mov    r15, [reg_p1+56]
	adc    r14, [reg_p1+128]
	adc    r15, [reg_p1+136]
	mov    r9, [reg_p1+64]
	mov    r10, [reg_p1+72]
	adc    r9, [reg_p1+144]
	adc    r10, [reg_p1+152]
	mov    [reg_p2+24], r11
	mov    [reg_p2+32], r12
	mov    [reg_p2+40], r13
	mov    [reg_p2+48], r14
	mov    [reg_p2+56], r15
	mov    [reg_p2+64], r9
	mov    [reg_p2+72], r10

	// a0 - a1 + 4xp610
	mov    rcx, [reg_p1]
	mov    r10, [reg_p1+8]
	mov    r12, [reg_p1+16]
	mov    r13, [reg_p1+24]
	mov    r14, [reg_p1+32]
	mov    r15, [reg_p1+40]
	sub    rcx, [reg_p1+80]
	sbb    r10, [reg_p1+88]
	sbb    r12, [reg_p1+96]
	sbb    r13, [reg_p1+104]
	sbb    r14, [reg_p1+112]
	sbb    r15, [reg_p1+120]
	mov    rbx, [reg_p1+48]
	mov    rbp, [reg_p1+56]
	mov    r8, [reg_p1+64]
	mov    rax, [reg_p1+72]
	sbb    rbx, [reg_p1+128]
	sbb    rbp, [reg_p1+136]
	sbb    r8, [reg_p1+144]
	sbb    rax, [reg_p1+152]
	add    rcx, [rip+prime_modulus_x4]
	mov    rdx, [rip+prime_modulus_x4+8]
	adc    r10, rdx
	adc    r12, rdx
	adc    r13, rdx
	adc    r14, [rip+prime_modulus_x4+32]
	adc    r15, [rip+prime_modulus_x4+40]
	adc    rbx, [rip+prime_modulus_x4+48]
	adc    rbp, [rip+prime_modulus_x4+56]
	adc    r8, [rip+prime_modulus_x4+64]
	adc    rax, [rip+prime_modulus_x4+72]
	mov    [reg_p2+80], rcx
	mov    [reg_p2+88], r10
	mov    [reg_p2+96], r12
	mov    [reg_p2+104], r13
	mov    [reg_p2+112], r14
	mov    [reg_p2+144], r8
	mov    [reg_p2+152], rax

    // [rsp0:rsp16, r11:r15, r8:r10] <- z = a00 x a1
    mov    rdx, [reg_p2]
    mulx   r9, r8, rcx
	mov    [reg_p2+120], r15
    xor    rax, rax
    mulx   r10, r11, r10
	mov    [reg_p2+128], rbx
    adcx   r9, r11
    mulx   r11, r12, r12
	mov    [reg_p2+136], rbp
    adcx   r10, r12
    mulx   r12, r13, r13
	mov    [rsp+24], r8
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
    adcx   r15, r10
    mulx   r9, rax, [reg_p2+144]
    adcx   r8, rax
    mulx   r10, rbx, [reg_p2+152]
    adcx   r9, rbx
    adc    r10, 0

	FPMUL640x640 [reg_p2], [reg_p2+80], [rsp], [reg_p2], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rbp, rax

    mov    [reg_p2+24], r13
    mov    [reg_p2+32], r14
    mov    [reg_p2+40], r15
    mov    [reg_p2+48], r8
    mov    [reg_p2+56], r9
    mov    [reg_p2+64], r10
    mov    [reg_p2+72], r11
	add    rsp, 32
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
	adc    rbx, rbx
	adc    rbp, rbp
	sub    rsp, 112
	mov    [rsp+8], r9
	mov    [rsp+16], r10
	mov    [rsp+24], r11

    // [rsp24, rsp0:rsp16, r11:r15, r8:r10] <- z = a00 x a1
    mov    rdx, r8
    mulx   r9, r8, [reg_p1+80]
	mov    [rsp+32], r12
    xor    rax, rax
    mulx   r10, r11, [reg_p1+88]
	mov    [rsp+40], r13
    adcx   r9, r11
    mulx   r11, r12, [reg_p1+96]
	mov    [rsp+48], r14
    adcx   r10, r12
    mulx   r12, r13, [reg_p1+104]
	mov    [rsp+104], r8
    adcx   r11, r13
    mulx   r13, r8, [reg_p1+112]
	mov    [rsp+80], r9
    adcx   r12, r8
    mulx   r14, r9, [reg_p1+120]
	mov    [rsp+56], r15
    adcx   r13, r9
    mulx   r15, rax, [reg_p1+128]
	mov    [rsp+88], r10
    adcx   r14, rax
    mulx   r8, r10, [reg_p1+136]
	mov    [rsp+96], r11
    adcx   r15, r10
    mulx   r9, rax, [reg_p1+144]
	mov    [rsp+64], rbx
    adcx   r8, rax
    mulx   r10, rbx, [reg_p1+152]
	mov    [rsp+72], rbp
    adcx   r9, rbx
    adc    r10, 0

	FPMUL640x640 [rsp], [reg_p1+80], [rsp+80], [reg_p2], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rbp, rax

    mov    [reg_p2+24], r13
    mov    [reg_p2+32], r14
    mov    [reg_p2+40], r15
    mov    [reg_p2+48], r8
    mov    [reg_p2+56], r9
    mov    [reg_p2+64], r10
    mov    [reg_p2+72], r11
	add    rsp, 112
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
	sub    rsp, 32

    // [r8:r15] <- z = a x b0
    mov    rdx, [reg_p3 + 0]
    mulx   r9, r8, [reg_p2]
    xor    rax, rax
	mov    [rsp+24], r8
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
    adcx   r9, rbx
    adc    r10, 0

	FPMUL640x640 [rcx], [reg_p2], [rsp], [reg_p1], r8, r9, r10, r11, r12, r13, r14, r15, rbx, rbp, rax

    mov    [reg_p1+24], r13
    mov    [reg_p1+32], r14
    mov    [reg_p1+40], r15
    mov    [reg_p1+48], r8
    mov    [reg_p1+56], r9
    mov    [reg_p1+64], r10
    mov    [reg_p1+72], r11
	add    rsp, 32
    pop    rbp
    pop    rbx
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    rdx
    jmp .reduce_once


.global fmt(prime_field_square)
fmt(prime_field_square):
	mov rdx, rsi
	jmp fmt(prime_field_multiplication)
