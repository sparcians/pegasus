/* Test: misalgn.elf
 * ISA: rv64i
 * Description: Stream of misaligned loads and stores.
 */

.section .text
    .global main

main:
    # First double-word access will be aligned (0x1ff8)
    # Each subsequent access with be misligned by +1B
    # Final access will be completely on the next page (0x2000)
    lui x3, 1
    addi x6, x3, -8
    add x6, x6, x3

    # Init x5 to 0xaaccaacc
    addi x29, x29, 8
    addi x5, x5, 0xaa
    sll x5, x5, x29
    addi x5, x5, 0xcc
    sll x5, x5, x29
    addi x5, x5, 0xaa
    sll x5, x5, x29
    addi x5, x5, 0xcc
    sll x5, x5, x29
    addi x5, x5, 0xaa
    sll x5, x5, x29
    addi x5, x5, 0xcc
    sll x5, x5, x29
    addi x5, x5, 0xaa
    sll x5, x5, x29
    addi x5, x5, 0xcc

    # Counter for loop
    addi x28, x0, -9

loop:
    sd x5, 0(x6)
    ld x7, 0(x6)
    addi x6, x6, 1
    addi x28, x28, 1
    blt x28, zero, loop
    wfi

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
