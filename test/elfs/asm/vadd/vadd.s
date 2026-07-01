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
    la a0, data1
    vle8.v v2, (a0)
    la a0, data2
    vle8.v v3, (a0)
    vadd.vv	v1,v2,v3
    vsetvli t0, x0, e8, m2, ta, ma
    vadd.vv	v2,v4,v6
    vsetvli t0, x0, e8, m4, ta, ma
    vadd.vv	v2,v6,v10
    vsetvli t0, x0, e8, m8, ta, ma
    vadd.vv	v0,v8,v16

pass:
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
data1:
    .fill 64, 4, 0x1
data2:
    .fill 64, 4, 0x2
