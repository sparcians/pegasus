/* Test: add.elf
 * ISA: rv64i
 * Description: Stream of reg-reg load upper immediate instruction
 *.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

main:
    .rept 200
        lui x5, 0xFFFFF
        lui x5, 0x00001
        lui x5, 0x7FFFF
        lui x5, 0x00000
        lui x5, 0xF0000
    .endr

pass:
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
