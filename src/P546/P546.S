.intel_syntax noprefix

// Format function and variable names for Mac OS X
#if defined(__APPLE__)
	#define fmt(f)	_##f
#else
	#define fmt(f)	f
#endif

.set pbits,546
.set pbytes,72
.set plimbs,9

.global prime_modulus
prime_modulus:
	.quad 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF 
	.quad 0xC1CCF59098E1FFFF, 0x91CA3591A0810F4F, 0xC3A747738CBAAD7D, 0x3E568459654D5F6B 
	.quad 0x30F5EBA42 


.text
.p2align 4,,15

.global multiprecision_addition_asm
multiprecision_addition_asm:
	mov rax, [rsi +  0]
	add rax, [rdx +  0]
	mov [rdi +  0], rax
	.set k, 1
	.rept 8
		mov rax, [rsi + 8*k]
		adc rax, [rdx + 8*k]
		mov [rdi + 8*k], rax
		.set k, k+1
	.endr
	setc al
	movzx rax, al
	ret

.global multiprecision_subtraction_asm
multiprecision_subtraction_asm:
	mov rax, [rsi +  0]
	sub rax, [rdx +  0]
	mov [rdi +  0], rax
	.set k, 1

	.rept 8
		mov rax, [rsi + 8*k]
		sbb rax, [rdx + 8*k]
		mov [rdi + 8*k], rax
		.set k, k+1
	.endr
	setc al
	movzx rax, al
	ret


.reduce_once:
	push rbp
	push r12
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

	pop r12
	pop rbp
	ret

.global fmt(prime_field_addition)
fmt(prime_field_addition):
	push rdi
	call multiprecision_addition_asm
	pop rdi
	jmp .reduce_once

.global fmt(prime_field_subtraction)
fmt(prime_field_subtraction):
	push rdi
	call multiprecision_subtraction_asm
	pop rdi
	sub rsp, pbytes

	xor r8, r8
	cmp r8, rax
	cmovne r8, [rip + prime_modulus + 0]
	mov [rsp + 0], r8

	.set k, 1
	.rept plimbs-1
		mov rax, [rip + prime_modulus + 8*k]
		and rax, r8
		mov [rsp + 8*k], rax
		.set k, k+1
	.endr

	mov rax, [rsp +  0]
	add [rdi +  0], rax
	.set k, 1
	.rept plimbs-1
		mov rax, [rsp + 8*k]
		adc [rdi + 8*k], rax
		.set k, k+1
	.endr

	add rsp, pbytes
	ret

.global fmt(prime_field_negate)
fmt(prime_field_negate):
	sub rsp, pbytes

	xor r8, r8
	mov rax, [rsi +  0]
	mov [rdi + 0], r8
	sub [rdi +  0], rax

	.set k, 1
	.rept plimbs-1
		mov rax, [rsi +  8*k]
		mov [rdi + 8*k], r8
		sbb [rdi +  8*k], rax
	.set k, k+1
	.endr
	setc al
	movzx rax, al
	xor r8, r8
	cmp r8, rax
	cmovne r8, [rip + prime_modulus + 0]
	mov [rsp + 0], r8

	.set k, 1
	.rept plimbs-1
		mov rax, [rip + prime_modulus + 8*k]
		and rax, r8
		mov [rsp + 8*k], rax
		.set k, k+1
	.endr

	mov rax, [rsp +  0]
	add [rdi +  0], rax
	.set k, 1
	.rept plimbs-1
		mov rax, [rsp + 8*k]
		adc [rdi + 8*k], rax
		.set k, k+1
	.endr

	add rsp, pbytes
	ret

