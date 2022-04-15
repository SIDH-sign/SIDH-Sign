.intel_syntax noprefix

// Format function and variable names for Mac OS X
#if defined(__APPLE__)
	#define fmt(f)	_##f
#else
	#define fmt(f)	f
#endif

.set pbits,751
.set pbytes,96
.set plimbs,12

.global prime_modulus
prime_modulus:
	.quad 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF 
	.quad 0xFFFFFFFFFFFFFFFF, 0xEEAFFFFFFFFFFFFF, 0xE3EC968549F878A8, 0xDA959B1A13F7CC76 
	.quad 0x84E9867D6EBE876, 0x8562B5045CB25748, 0xE12909F97BADC66, 0x6FE5D541F71C 


.text
.p2align 4,,15

.global multiprecision_addition_asm
multiprecision_addition_asm:
	mov rax, [rsi +  0]
	add rax, [rdx +  0]
	mov [rdi +  0], rax
	.set k, 1
	.rept 11
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

	.rept 11
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
	push r13
	push r14
	push r15
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
	mov r14, [rbp +  80]
	sbb r14, [rip + prime_modulus + 80]
	mov r15, [rbp +  88]
	sbb r15, [rip + prime_modulus + 88]

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
	cswap2 r14, [rbp +  80]
	cswap2 r15, [rbp +  88]

	pop r15
	pop r14
	pop r13
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

.global fmt(prime_field_multiplication)
fmt(prime_field_multiplication):
	push rbp
	push rbx

	sub rsp, 112
	mov [rsp + 104], rdi
	mov rdi,rsi
	mov rsi,rdx

	xor rax,rax
	mov [rsp + 0], rax
	mov [rsp + 8], rax
	mov [rsp + 16], rax
	mov [rsp + 24], rax
	mov [rsp + 32], rax
	mov [rsp + 40], rax
	mov [rsp + 48], rax
	mov [rsp + 56], rax
	mov [rsp + 64], rax
	mov [rsp + 72], rax
	mov [rsp + 80], rax
	mov [rsp + 88], rax
	mov [rsp + 96], rax

.macro MULSTEP, k, I0,I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12
	mov r11,[rsp + \I0]
	mov rdx, [rsi +  0]
	mulx rcx, rdx, [rdi + 8*\k]
	add rdx, r11
	xor rax, rax /* clear flags */

	mulx rbx, rax, [rip + prime_modulus +  0]
	adox r11, rax
	mov [rsp + \I0], r11

	mov r11,[rsp + \I1]
	mulx rcx, rax, [rip + prime_modulus + 8]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I1], r11

	mov r11,[rsp + \I2]
	mulx rbx, rax, [rip + prime_modulus + 16]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I2], r11

	mov r11,[rsp + \I3]
	mulx rcx, rax, [rip + prime_modulus + 24]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I3], r11

	mov r11,[rsp + \I4]
	mulx rbx, rax, [rip + prime_modulus + 32]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I4], r11

	mov r11,[rsp + \I5]
	mulx rcx, rax, [rip + prime_modulus + 40]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I5], r11

	mov r11,[rsp + \I6]
	mulx rbx, rax, [rip + prime_modulus + 48]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I6], r11

	mov r11,[rsp + \I7]
	mulx rcx, rax, [rip + prime_modulus + 56]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I7], r11

	mov r11,[rsp + \I8]
	mulx rbx, rax, [rip + prime_modulus + 64]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I8], r11

	mov r11,[rsp + \I9]
	mulx rcx, rax, [rip + prime_modulus + 72]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I9], r11

	mov r11,[rsp + \I10]
	mulx rbx, rax, [rip + prime_modulus + 80]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I10], r11

	mov r11,[rsp + \I11]
	mulx rcx, rax, [rip + prime_modulus + 88]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I11], r11

	mov r11,[rsp + \I12]
	mov rax, 0
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I12], r11

	mov rdx, [rdi + 8*\k]

	xor rax, rax /* clear flags */

	mov r11,[rsp + \I0]
	mulx rbx, rax, [rsi +  0]
	adox r11, rax
	mov [rsp + \I0], r11

	mov r11, [rsp + \I1]
	mulx rcx, rax, [rsi + 8]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I1], r11

	mov r11, [rsp + \I2]
	mulx rbx, rax, [rsi + 16]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I2], r11

	mov r11, [rsp + \I3]
	mulx rcx, rax, [rsi + 24]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I3], r11

	mov r11, [rsp + \I4]
	mulx rbx, rax, [rsi + 32]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I4], r11

	mov r11, [rsp + \I5]
	mulx rcx, rax, [rsi + 40]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I5], r11

	mov r11, [rsp + \I6]
	mulx rbx, rax, [rsi + 48]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I6], r11

	mov r11, [rsp + \I7]
	mulx rcx, rax, [rsi + 56]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I7], r11

	mov r11, [rsp + \I8]
	mulx rbx, rax, [rsi + 64]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I8], r11

	mov r11, [rsp + \I9]
	mulx rcx, rax, [rsi + 72]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I9], r11

	mov r11, [rsp + \I10]
	mulx rbx, rax, [rsi + 80]
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I10], r11

	mov r11, [rsp + \I11]
	mulx rcx, rax, [rsi + 88]
	adcx r11, rbx
	adox r11, rax
	mov [rsp + \I11], r11

	mov r11, [rsp + \I12]
	mov rax, 0
	adcx r11, rcx
	adox r11, rax
	mov [rsp + \I12], r11

.endm

MULSTEP 0,8,16,24,32,40,48,56,64,72,80,88,96,0
MULSTEP 1,16,24,32,40,48,56,64,72,80,88,96,0,8
MULSTEP 2,24,32,40,48,56,64,72,80,88,96,0,8,16
MULSTEP 3,32,40,48,56,64,72,80,88,96,0,8,16,24
MULSTEP 4,40,48,56,64,72,80,88,96,0,8,16,24,32
MULSTEP 5,48,56,64,72,80,88,96,0,8,16,24,32,40
MULSTEP 6,56,64,72,80,88,96,0,8,16,24,32,40,48
MULSTEP 7,64,72,80,88,96,0,8,16,24,32,40,48,56
MULSTEP 8,72,80,88,96,0,8,16,24,32,40,48,56,64
MULSTEP 9,80,88,96,0,8,16,24,32,40,48,56,64,72
MULSTEP 10,88,96,0,8,16,24,32,40,48,56,64,72,80
MULSTEP 11,96,0,8,16,24,32,40,48,56,64,72,80,88

	mov rdi, [rsp + 104]

	mov r11, [rsp + 0]
	mov [rdi + 0], r11
	mov r11, [rsp + 8]
	mov [rdi + 8], r11
	mov r11, [rsp + 16]
	mov [rdi + 16], r11
	mov r11, [rsp + 24]
	mov [rdi + 24], r11
	mov r11, [rsp + 32]
	mov [rdi + 32], r11
	mov r11, [rsp + 40]
	mov [rdi + 40], r11
	mov r11, [rsp + 48]
	mov [rdi + 48], r11
	mov r11, [rsp + 56]
	mov [rdi + 56], r11
	mov r11, [rsp + 64]
	mov [rdi + 64], r11
	mov r11, [rsp + 72]
	mov [rdi + 72], r11
	mov r11, [rsp + 80]
	mov [rdi + 80], r11
	mov r11, [rsp + 88]
	mov [rdi + 88], r11
	add rsp, 112

	pop rbx
	pop rbp
	jmp .reduce_once

.global fmt(prime_field_square)
fmt(prime_field_square):
	mov rdx, rsi
	jmp fmt(prime_field_multiplication)

