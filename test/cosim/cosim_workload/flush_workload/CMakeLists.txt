project(FlushWorkload_Test)

add_executable(FlushWorkload_test FlushWorkload_test.cpp)

file (CREATE_LINK ${SIM_BASE}/arch               ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${SIM_BASE}/mavis/json         ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${SIM_BASE}/test/sim/workloads ${CMAKE_CURRENT_BINARY_DIR}/workloads SYMBOLIC)

# Quick-running tests for "make pegasus_regress"
file (CREATE_LINK ${SIM_BASE}/test/cosim/cosim_workload/rv64mi-p-csr ${CMAKE_CURRENT_BINARY_DIR}/rv64mi-p-csr SYMBOLIC)
cosim_named_test(CoSimFlushWorkload_test_run FlushWorkload_test -w rv64mi-p-csr -p top.core0.params.isa rv64imafdcbv_zicsr_zifencei_zicond_zfh)
set (LINUX_ARCH_SETUP --reg "core0.hart0.sp 0x0000003ffffff000"
                      --reg "core0.hart0.gp 0x77000"
                      --reg "core0.hart0.tp 0x7d000" -p top.extension.sim.enable_syscall_emulation true)
cosim_named_test(CoSimFlushSysCall_test_run FlushWorkload_test -w workloads/dhry.elf -i 10000 ${LINUX_ARCH_SETUP})

# Exhaustive test for "make pegasus_cosim_regress" to run all ISA tests in parallel
find_package(Python3 REQUIRED)
add_custom_target(pegasus_cosim_regress
    COMMAND ${CMAKE_COMMAND} -E echo "Running full cosimulation regression suite..."
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/RunArchTests.py
        --rv64-only --riscv-arch ${CMAKE_SOURCE_DIR}/riscv-tests
        --pegasus-exe ${CMAKE_CURRENT_BINARY_DIR}/FlushWorkload_test
        --expected-pass-rate 100
    COMMAND ${CMAKE_COMMAND} -E rm -f *.log *.db *.db-journal
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
    DEPENDS FlushWorkload_test
)
