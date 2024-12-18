/* Test: add.elf
 * ISA: rv64i
 * Description: Stream of reg-reg sub instructions.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

main:
    addi x28, x0, 10
    addi x29, x0, 1
loop:
    .rept 100
        sub x30, x28, x29
    .endr
    addi x29, x29, 1
    bgeu x28, x29, loop

pass:
    test_pass

fail:
    test_fail

.section .data
data:
     .fill 64, 4, 0xFFFFFFFF
