/* Test: nop.elf
 * ISA: rv64i
 * Description: Stream of nops.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

main:
    add.uw x0, x1, x2
    andn x0, x1, x2
    clmul x0, x1, x2
    clmulh x0, x1, x2
    clmulr x0, x1, x2
    clz x0, x2
    clzw x0, x2
    cpop x0, x2
    cpopw x0, x2
    ctz x0, x2
    ctzw x0, x2
    max x0, x1, x2
    maxu x0, x1, x2
    min x0, x1, x2
    minu x0, x1, x2
    orc.b x0, x1
    orn x0, x1, x2
    rev8 x0, x2
    rol x0, x1, x2
    rolw x0, x1, x2
    ror x0, x1, x2
    rori x0, x1, 1
    roriw x0, x1, 1
    rorw x0, x1, x2
    bclr x0, x1, x2
    bclri x0, x1, 1
    bext x0, x1, x2
    bexti x0, x1, 1
    binv x0, x1, x2
    binvi x0, x1, 1
    bset x0, x1, x2
    bseti x0, x1, 1
    sext.b x0, x2
    sext.h x0, x2
    sh1add x0, x1, x2
    sh1add.uw x0, x1, x2
    sh2add x0, x1, x2
    sh2add.uw x0, x1, x2
    sh3add x0, x1, x2
    sh3add.uw x0, x1, x2
    slli.uw x0, x1, 1
    xnor x0, x1, x2
    zext.h x0, x2

pass:
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
