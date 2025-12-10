project(pegasus_test)

# This line will make sure pegasus is built before running the tests
pegasus_regress (pegasus)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/workloads                      ${CMAKE_CURRENT_BINARY_DIR}/workloads SYMBOLIC)

# ASM Tests
pegasus_named_test(pegasus_nop_test pegasus -p top.core0.hart0.params.stop_sim_on_wfi true workloads/nop.elf)
pegasus_named_test(pegasus_uart_test pegasus -p top.system.params.enable_uart true workloads/uart.elf)

# Linux Tests
set (LINUX_ARCH_SETUP --reg "core0.hart0.sp 0x0000003ffffff000"
                      --reg "core0.hart0.gp 0x77000"
                      --reg "core0.hart0.tp 0x7d000" -p top.extension.sim.enable_syscall_emulation true)

set (SYSCALL_LOG -l top syscall ${CMAKE_CURRENT_BINARY_DIR}/syscall.log -l top inst ${CMAKE_CURRENT_BINARY_DIR}/inst.log)

set (TEST_TEXT_FILE ${PROJECT_SOURCE_DIR}/../elfs/linux/syscall_test/test_text.txt)
pegasus_named_test(pegasus_dhry_test pegasus ${LINUX_ARCH_SETUP} workloads/dhry.elf)
pegasus_named_test(pegasus_fstatat_test pegasus ${LINUX_ARCH_SETUP} "workloads/fstatat_test.elf ${TEST_TEXT_FILE} 0" )
pegasus_named_test(pegasus_syscall_test pegasus ${LINUX_ARCH_SETUP} "workloads/syscall_test.elf ${TEST_TEXT_FILE}" )

# Logging tests
pegasus_named_test(pegasus_inst_logger_test pegasus -l top inst nop.instlog workloads/nop.elf)
pegasus_named_test(spike_inst_logger_test pegasus -l top inst nop.instlog --spike-formatting workloads/nop.elf)

# Multihart test
pegasus_named_test(pegasus_multihart_test pegasus -p top.core0.params.num_harts 2 -p top.core0.hart1.params.hart_id 1 workloads/multihart.elf workloads/multihart.elf)
