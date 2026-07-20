project(ECachePrelim_Test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)

add_executable(ECachePrelim_test ECachePrelim_test.cpp)
target_link_libraries(ECachePrelim_test pegasussim)
add_executable(ECachePerf_test ECachePerf_test.cpp)
target_link_libraries(ECachePerf_test pegasussim)

pegasus_named_test(ECachePrelim_test_run ECachePrelim_test)
pegasus_named_test(ECachePerf_test_run ECachePerf_test)
