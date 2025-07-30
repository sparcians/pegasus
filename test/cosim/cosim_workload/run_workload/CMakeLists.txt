project(RunWorkload_Test)

add_executable(RunWorkload_test RunWorkload_test.cpp)

file (CREATE_LINK ${CMAKE_SOURCE_DIR}/arch       ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${CMAKE_SOURCE_DIR}/mavis/json ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${CMAKE_SOURCE_DIR}/core/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

file (CREATE_LINK ${CMAKE_SOURCE_DIR}/test/cosim/cosim_workload/rv64mi-p-csr ${CMAKE_CURRENT_BINARY_DIR}/rv64mi-p-csr SYMBOLIC)
cosim_named_test(RunWorkload_test_run RunWorkload_test rv64mi-p-csr)
