project(FlushWorkload_Test)

add_executable(FlushWorkload_test FlushWorkload_test.cpp)

file (CREATE_LINK ${SIM_BASE}/arch       ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${SIM_BASE}/mavis/json ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${SIM_BASE}/core/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

# Quick-running test for "make pegasus_regress"
file (CREATE_LINK ${SIM_BASE}/test/cosim/cosim_workload/rv64mi-p-csr ${CMAKE_CURRENT_BINARY_DIR}/rv64mi-p-csr SYMBOLIC)
cosim_named_test(FlushWorkload_test_run FlushWorkload_test -w rv64mi-p-csr)

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
