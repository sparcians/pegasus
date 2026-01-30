/* Test: wrs_nto.elf
 * ISA: rv64zawrs
 * Description:
 *   The test presumes hart1 runs after hart0 is paused.
 *   Otherwise hart0 will never receive the wakening trigger
 *   from hart 1.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

# Increment thread count
stop_running:
    la x6,data
    lw x7, 0(x6)
    addi x8, x7, 0x1
    sw x8, 0(x6)
loop:
# Check if both threads have finished
    pause
    lw x7, 0(x6)
    addi x8, x0, 0x2
    bne x7, x8, loop
    j pass

main:
    csrrs x28,mhartid,x0
    bnez x28,hart1

hart0:
    la x10, scratch
    lr.w x11, (x10)
    wrs.nto
    j stop_running

hart1:
    la x10, scratch
    li x11, 1
    sw x11, 0(x10)
    j stop_running

pass:
    test_pass

.section .data
data:
    .fill 64, 4, 0x00000000
scratch:
    .fill 64, 4, 0x00000000
