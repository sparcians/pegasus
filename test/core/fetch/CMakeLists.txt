project(Fetch_Test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

add_executable(Fetch_test Fetch_test.cpp)
target_link_libraries(Fetch_test pegasuscore pegasusinsts softfloat pegasussys pegasussim pegasussys ${PEGASUS_LIBS})

pegasus_named_test(Fetch_test_run Fetch_test)
