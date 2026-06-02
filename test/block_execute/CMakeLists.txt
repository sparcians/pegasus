project(BlockExecute_Test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)

add_executable(BlockExecute_test BlockExecute_test.cpp)
target_link_libraries(BlockExecute_test pegasussim)

pegasus_named_test(BlockExecute_test_run BlockExecute_test)
