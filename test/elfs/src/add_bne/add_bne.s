/* Test: add.elf
 * ISA: rv64i
 * Description: Stream of reg-reg add instructions with a branch.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

main:
    addi x28, x0, 10
    addi x7, x0, 1
loop:
    .rept 100
        add x5, x5, x7
    .endr
    addi x28, x28, -1
    bne x28, zero, loop

pass:
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
