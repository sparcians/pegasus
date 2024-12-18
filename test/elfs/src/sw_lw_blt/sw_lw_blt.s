/* Test: add.elf
 * ISA: rv64i
 * Description: Stream of store and load instructions.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

main:
    # Temp - address of data is x1000
    lui x6, 1

    # Init x5: 0xaaccaacc
    addi x29, x29, 8
    addi x5, x5, 0xaa
    sll x5, x5, x29
    addi x5, x5, 0xcc
    sll x5, x5, x29
    addi x5, x5, 0xaa
    sll x5, x5, x29
    addi x5, x5, 0xcc

    # Counter for loop
    addi x28, x0, -10
loop:
    .rept 100
        sw x5, 0(x6)
        lw x7, 0(x6)
        addi x6, x6, 4
    .endr
    addi x28, x28, 1
    blt x28, zero, loop

pass:
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
