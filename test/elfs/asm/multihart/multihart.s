/* Test: multihart.elf
 * ISA:
 * Description:
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

# Increment counter for number of threads running
start_running:
    la x6,data
    lr.w x7, (x6)
    addi x8,x7,0x1
    sc.w x9, x8, (x6) # Try the write
    pause
    bnez x9, start_running
    jr ra # Return to main (start) if successful

# Decrement counter for number of threads running
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
    csrrs x28,mhartid,x0
    bnez x28,hart1

hart0:
    nop
    j stop_running

hart1:
    nop
    j stop_running

# Number of running threads must be zero for EOT
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
