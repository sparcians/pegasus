project(RunWorkload_Test)

add_executable(RunWorkload_test RunWorkload_test.cpp)

file (CREATE_LINK ${CMAKE_SOURCE_DIR}/arch       ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${CMAKE_SOURCE_DIR}/mavis/json ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${CMAKE_SOURCE_DIR}/core/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

cosim_named_test(RunWorkload_test_run RunWorkload_test /home/cnyce/atlas-fork/riscv-tests/isa/rv64mi-p-csr)
