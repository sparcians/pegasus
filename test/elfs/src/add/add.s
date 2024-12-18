/* Test: add.elf
 * ISA: rv64i
 * Description: Stream of reg-reg add instructions.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

main:
    start_tracepoint
    .rept 1000
        add x7, x6, x5
    .endr
    stop_tracepoint

pass:
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
