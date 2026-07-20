/* Test: nop.elf
 * ISA: rv64i
 * Description: Stream of nops.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

main:
    vsetvli t0, x0, e8, m1, ta, ma
    vmandn.mm	v14,v30,v22
    vsetvli t0, x0, e8, m2, ta, ma
    vmandn.mm	v14,v30,v22
    vsetvli t0, x0, e8, m4, ta, ma
    vmandn.mm	v14,v30,v22
    vsetvli t0, x0, e8, m8, ta, ma
    vmandn.mm	v14,v30,v22

pass:
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
