project(FlushWorkload_Test)

add_executable(FlushWorkload_test FlushWorkload_test.cpp)

file (CREATE_LINK ${SIM_BASE}/arch       ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${SIM_BASE}/mavis/json ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${SIM_BASE}/core/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

file (CREATE_LINK ${SIM_BASE}/test/cosim/cosim_workload/rv64mi-p-csr ${CMAKE_CURRENT_BINARY_DIR}/rv64mi-p-csr SYMBOLIC)
cosim_named_test(FlushWorkload_test_run FlushWorkload_test rv64mi-p-csr)
