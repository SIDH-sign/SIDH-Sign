#!/usr/bin/env python3
import click


# +++++++++++++++++++++++
def xgcd(a: int, b: int):
    """
    Extended GCD
    :param a: integer number
    :param b: integer number
    :return: Returns (gcd, x, y) where gcd is the greatest common divisor of a and b with the sign of b if b is nonzero,
    and with the sign of a if b is 0 . The numbers x and y are such that gcd = ax+by
    """
    prev_x, x = 1, 0
    prev_y, y = 0, 1
    while b:
        q, r = divmod(a, b)
        x, prev_x = prev_x - q * x, x
        y, prev_y = prev_y - q * y, y
        a, b = b, r
    return a, prev_x, prev_y


# ++++++++++++++++++++++++++++++++++
def word_extractor(p, p_words, var):
    k = 64
    p_word = []
    content_str = ".global " + var + "\n" + var + ":\n"
    pp = p
    for i in range(1, p_words + 1):
        if ((i - 1) % 4) == 0:
            content_str = content_str + "\t.quad "
        p_word.append(pp % 2 ** k)
        if (i % 4) != 0 and (i != p_words):
            content_str = content_str + "0x{:X}".format(p_word[i - 1]) + ", "
        else:
            content_str = content_str + "0x{:X}".format(p_word[i - 1]) + " \n"
        pp = pp // (2 ** k)

    content_str = content_str + "\n"
    click.echo(f'{content_str}')


# ++++++++++++++++++++++++++++++++++++++
def print_header(pbits, pbytes, plimbs):
    click.echo('.intel_syntax noprefix\n')
    click.echo('// Format function and variable names for Mac OS X')
    click.echo('#if defined(__APPLE__)')
    click.echo('\t#define fmt(f)\t_##f')
    click.echo('#else')
    click.echo('\t#define fmt(f)\tf')
    click.echo('#endif\n')
    click.echo(f'.set pbits,{pbits}')
    click.echo(f'.set pbytes,{pbytes}')
    click.echo(f'.set plimbs,{plimbs}\n')


# +++++++++++++++++++++++++++++++++++
def print_multiprecision_asm(plimbs):
    click.echo('.text\n.p2align 4,,15\n')
    # Multiprecision addition
    click.echo('.global multiprecision_addition_asm')
    click.echo('multiprecision_addition_asm:')
    click.echo('\tmov rax, [rsi +  0]')
    click.echo('\tadd rax, [rdx +  0]')
    click.echo('\tmov [rdi +  0], rax')
    click.echo('\t.set k, 1')
    click.echo(f'\t.rept {plimbs - 1}')
    click.echo('\t\tmov rax, [rsi + 8*k]')
    click.echo('\t\tadc rax, [rdx + 8*k]')
    click.echo('\t\tmov [rdi + 8*k], rax')
    click.echo('\t\t.set k, k+1')
    click.echo('\t.endr')
    click.echo('\tsetc al')
    click.echo('\tmovzx rax, al')
    click.echo('\tret\n')
    # Multiprecision subtraction
    click.echo('.global multiprecision_subtraction_asm')
    click.echo('multiprecision_subtraction_asm:')
    click.echo('\tmov rax, [rsi +  0]')
    click.echo('\tsub rax, [rdx +  0]')
    click.echo('\tmov [rdi +  0], rax')
    click.echo('\t.set k, 1\n')
    click.echo(f'\t.rept {plimbs - 1}')
    click.echo('\t\tmov rax, [rsi + 8*k]')
    click.echo('\t\tsbb rax, [rdx + 8*k]')
    click.echo('\t\tmov [rdi + 8*k], rax')
    click.echo('\t\t.set k, k+1')
    click.echo('\t.endr')
    click.echo('\tsetc al')
    click.echo('\tmovzx rax, al')
    click.echo('\tret\n')


# ++++++++++++++++++++++++++++
def print_reduce_once(pbytes, plimbs):
    reg_ar = ["rdi", "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]

    click.echo('\n.reduce_once:')
    click.echo('\tpush rbp')
    for i in range(8, plimbs, 1):
        click.echo(f'\tpush {reg_ar[i % len(reg_ar)]}')

    click.echo('\tmov rbp, rdi\n')
    click.echo('\tmov rdi, [rbp +  0]')
    click.echo('\tsub rdi, [rip + prime_modulus +  0]')

    N = pbytes - 8
    for i in range(8, N + 1, 8):
        click.echo(f'\tmov {reg_ar[(int(i / 8) % len(reg_ar))]}, [rbp +  {i}]')
        click.echo(f'\tsbb {reg_ar[(int(i / 8) % len(reg_ar))]}, [rip + prime_modulus + {i}]')
        if ((int((i + 8) / 8) % 8) == 0) and (i != 0):
            click.echo('')

    click.echo('\n\tsetnc al')
    click.echo('\tmovzx rax, al')
    click.echo('\tneg rax\n')
    click.echo('.macro cswap2, r, m')
    click.echo('\txor \\r, \\m')
    click.echo('\tand \\r, rax')
    click.echo('\txor \\m, \\r')
    click.echo('.endm\n')

    for i in range(0, N + 1, 8):
        click.echo(f'\tcswap2 {reg_ar[(int(i / 8) % len(reg_ar))]}, [rbp +  {i}]')
        if ((int((i + 8) / 8) % 8) == 0) and (i != 0):
            click.echo('')

    click.echo('')
    for i in range(plimbs - 1, 7, -1):
        click.echo(f'\tpop {reg_ar[i % len(reg_ar)]}')
    click.echo('\tpop rbp')
    click.echo('\tret\n')


# +++++++++++++++++++++++++++++++++++
def print_addition_subtraction_and_negate():
    # Prime field addition
    click.echo('.global fmt(prime_field_addition)')
    click.echo('fmt(prime_field_addition):')
    click.echo('\tpush rdi')
    click.echo('\tcall multiprecision_addition_asm')
    click.echo('\tpop rdi')
    click.echo('\tjmp .reduce_once\n')

    # Prime field subtraction
    click.echo('.global fmt(prime_field_subtraction)')
    click.echo('fmt(prime_field_subtraction):')
    click.echo('\tpush rdi')
    click.echo('\tcall multiprecision_subtraction_asm')
    click.echo('\tpop rdi')
    click.echo('\tsub rsp, pbytes\n')

    click.echo('\txor r8, r8')
    click.echo('\tcmp r8, rax')
    click.echo('\tcmovne r8, [rip + prime_modulus + 0]')
    click.echo('\tmov [rsp + 0], r8\n')

    click.echo('\t.set k, 1')
    click.echo('\t.rept plimbs-1')
    click.echo('\t\tmov rax, [rip + prime_modulus + 8*k]')
    click.echo('\t\tand rax, r8')
    click.echo('\t\tmov [rsp + 8*k], rax')
    click.echo('\t\t.set k, k+1')
    click.echo('\t.endr\n')
    click.echo('\tmov rax, [rsp +  0]')
    click.echo('\tadd [rdi +  0], rax')
    click.echo('\t.set k, 1')
    click.echo('\t.rept plimbs-1')
    click.echo('\t\tmov rax, [rsp + 8*k]')
    click.echo('\t\tadc [rdi + 8*k], rax')
    click.echo('\t\t.set k, k+1')
    click.echo('\t.endr\n')
    click.echo('\tadd rsp, pbytes')
    click.echo('\tret\n')

    # Prime field negate
    click.echo('.global fmt(prime_field_negate)')
    click.echo('fmt(prime_field_negate):')
    click.echo('\tsub rsp, pbytes\n')

    click.echo('\txor r8, r8')
    click.echo('\tmov rax, [rsi +  0]')
    click.echo('\tmov [rdi + 0], r8')
    click.echo('\tsub [rdi +  0], rax\n')

    click.echo('\t.set k, 1')
    click.echo('\t.rept plimbs-1')
    click.echo('\t\tmov rax, [rsi +  8*k]')
    click.echo('\t\tmov [rdi + 8*k], r8')
    click.echo('\t\tsbb [rdi +  8*k], rax')
    click.echo('\t.set k, k+1')
    click.echo('\t.endr')
    click.echo('\tsetc al')
    click.echo('\tmovzx rax, al')

    click.echo('\txor r8, r8')
    click.echo('\tcmp r8, rax')
    click.echo('\tcmovne r8, [rip + prime_modulus + 0]')
    click.echo('\tmov [rsp + 0], r8\n')

    click.echo('\t.set k, 1')
    click.echo('\t.rept plimbs-1')
    click.echo('\t\tmov rax, [rip + prime_modulus + 8*k]')
    click.echo('\t\tand rax, r8')
    click.echo('\t\tmov [rsp + 8*k], rax')
    click.echo('\t\t.set k, k+1')
    click.echo('\t.endr\n')
    click.echo('\tmov rax, [rsp +  0]')
    click.echo('\tadd [rdi +  0], rax')
    click.echo('\t.set k, 1')
    click.echo('\t.rept plimbs-1')
    click.echo('\t\tmov rax, [rsp + 8*k]')
    click.echo('\t\tadc [rdi + 8*k], rax')
    click.echo('\t\t.set k, k+1')
    click.echo('\t.endr\n')
    click.echo('\tadd rsp, pbytes')
    click.echo('\tret\n')


# +++++++++++++++++++++++++++
def print_multiplication_and_square(pbytes, pwords):
    reg_ar = ["rbx", "rcx"]

    # Prime field multiplication (schoolbook method)
    click.echo('.global fmt(prime_field_multiplication)')
    click.echo('fmt(prime_field_multiplication):')
    click.echo('\tpush rbp')
    click.echo('\tpush rbx\n')
    click.echo(f'\tsub rsp, {pbytes + 16}')
    click.echo(f'\tmov [rsp + {pbytes + 8}], rdi')
    click.echo('\tmov rdi,rsi')
    click.echo('\tmov rsi,rdx\n')
    click.echo('\txor rax,rax')

    N = pbytes

    for i in range(0, N + 1, 8):
        click.echo(f'\tmov [rsp + {i}], rax')

    R = ""
    for i in range(0, pwords):
        R = R + "I" + str(i) + ","

    R += "I" + str(pwords)
    click.echo(f'\n.macro MULSTEP, k, {R}')
    click.echo('\tmov r11,[rsp + \\I0]')
    click.echo('\tmov rdx, [rsi +  0]')
    click.echo('\tmulx rcx, rdx, [rdi + 8*\\k]')
    click.echo('\tadd rdx, r11')

    click.echo('\txor rax, rax /* clear flags */\n')
    click.echo('\tmulx rbx, rax, [rip + prime_modulus +  0]')
    click.echo('\tadox r11, rax')
    click.echo('\tmov [rsp + \\I0], r11\n')

    N = pbytes - 8

    for i in range(8, N + 1, 8):
        click.echo(f'\tmov r11,[rsp + \\I{int(i / 8)}]')
        click.echo(f'\tmulx {reg_ar[(int(i / 8) % 2)]}, rax, [rip + prime_modulus + {i}]')
        click.echo(f'\tadcx r11, {reg_ar[-(int(i / 8) % 2) + 1]}')
        click.echo('\tadox r11, rax')
        click.echo(f'\tmov [rsp + \\I{int(i / 8)}], r11\n')

    click.echo(f'\tmov r11,[rsp + \\I{pwords}]')
    click.echo('\tmov rax, 0')
    click.echo(f'\tadcx r11, {reg_ar[(pwords + 1) % 2]}')
    click.echo('\tadox r11, rax')
    click.echo(f'\tmov [rsp + \\I{pwords}], r11\n')
    click.echo('\tmov rdx, [rdi + 8*\\k]\n')
    click.echo('\txor rax, rax /* clear flags */\n')

    click.echo('\tmov r11,[rsp + \\I0]')
    click.echo('\tmulx rbx, rax, [rsi +  0]')
    click.echo('\tadox r11, rax')
    click.echo('\tmov [rsp + \\I0], r11\n')

    for i in range(8, N + 1, 8):
        click.echo(f'\tmov r11, [rsp + \\I{int(i / 8)}]')
        click.echo(f'\tmulx {reg_ar[int(i / 8) % 2]}, rax, [rsi + {i}]')
        click.echo(f'\tadcx r11, {reg_ar[-(int(i / 8) % 2) + 1]}')
        click.echo('\tadox r11, rax')
        click.echo(f'\tmov [rsp + \\I{int(i / 8)}], r11\n')

    click.echo(f'\tmov r11, [rsp + \\I{pwords}]')
    click.echo('\tmov rax, 0')
    click.echo(f'\tadcx r11, {reg_ar[(pwords + 1) % 2]}')
    click.echo('\tadox r11, rax')
    click.echo(f'\tmov [rsp + \\I{pwords}], r11\n')
    click.echo('.endm\n')

    T = ""
    for i in range(0, pwords):
        S = ""
        T = T + "MULSTEP " + str(i) + ","
        for j in range(8, pbytes + 1, 8):
            T = T + str((j + i * 8) % (pbytes + 8)) + ","
        S += T
        T = ""
        S += str((pbytes + 8 + i * 8) % (pbytes + 8))
        click.echo(f'{S}')

    click.echo(f'\n\tmov rdi, [rsp + {pbytes + 8}]\n')

    for i in range(0, N + 1, 8):
        click.echo(f'\tmov r11, [rsp + {i}]')
        click.echo(f'\tmov [rdi + {i}], r11')

    click.echo(f'\tadd rsp, {pbytes + 16}\n')
    click.echo('\tpop rbx')
    click.echo('\tpop rbp')
    click.echo('\tjmp .reduce_once\n')

    # Prime field square (not optimized)
    click.echo('.global fmt(prime_field_square)')
    click.echo('fmt(prime_field_square):')
    click.echo('\tmov rdx, rsi')
    click.echo('\tjmp fmt(prime_field_multiplication)\n')


# ++++++++++++++++++++++++++++++
def print_parameters(p, pwords):
    # Montgomery parameter R
    montgomery_parameter = 2 ** (64 * pwords)
    word_extractor(p, pwords, "prime_modulus")
    # // Computing -(1/p) mod R;
    p_neg = (-p) % montgomery_parameter
    true, pinv, _ = xgcd(p_neg, montgomery_parameter)
    assert (true == 1)
    pinv = pinv % montgomery_parameter
    inv_min_p_mod_r = pinv % (2 ** (64))


# ////////////////////////////////////////
def print_assembly(p):
    # // size of p in bits, bytes ,words
    pbits = p.bit_length()
    pbytes = (pbits + 7) // 8
    if (pbytes % 4) != 0:
        pbytes = pbytes + 4 - (pbytes % 4)
    pwords = (pbits + 63) // 64

    print_header(pbits, pbytes, pwords)
    print_parameters(p, pwords)
    print_multiprecision_asm(pwords)
    print_reduce_once(pbytes, pwords)
    print_addition_subtraction_and_negate()
    print_multiplication_and_square(pbytes, pwords)


@click.command()
@click.option('--a', default=191, help='Exponent of two')
@click.option('--b', default=117, help='Exponent of three')
def main(a: int, b: int):
    p = (2 ** a) * (3 ** b) - 1
    print_assembly(p)


# ++++++++++++++++++++++++
if __name__ == "__main__":
    main()
