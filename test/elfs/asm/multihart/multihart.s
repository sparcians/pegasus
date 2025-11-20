/* Test: multihart.elf
 * ISA:
 * Description:
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

start_running:
    la x6,data
    lr.w x7, (x6)
    addi x8,x7,0x1 # Increment num of running threads
    sc.w x9, x8, (x6) # Try to write
    pause
    bnez x9, start_running
    jr ra # Return (x1)

stop_running:
    la x6,data
    lr.w x7, (x6)
    addi x8,x7,-0x1
    sc.w x9, x8, (x6)
    pause
    bnez x9, stop_running
    j pass

main:
    jal start_running
start:
    la x6,data
    lr.w x7, (x6)
    addi x29,x0,0x2 # Number of threads running
    bne x7, x29, start
    csrrs x28,mhartid,x0
    bnez x28,hart1

hart0:
    nop
    j stop_running

hart1:
    nop
    j stop_running

pass:
    la x6,data
    lr.w x7, (x6)
    bnez x7, pass
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0x00000000
