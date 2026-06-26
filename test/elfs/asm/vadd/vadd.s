/* Test: nop.elf
 * ISA: rv64i
 * Description: Stream of nops.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

main:
    vsetvli t0, a0, e8, m1, ta, ma
    vadd.vv	v1,v2,v3
    vsetvli t0, a0, e8, m2, ta, ma
    vadd.vv	v2,v4,v6
    vsetvli t0, a0, e8, m4, ta, ma
    vadd.vv	v2,v6,v10
    vsetvli t0, a0, e8, m8, ta, ma
    vadd.vv	v0,v8,v16

pass:
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
