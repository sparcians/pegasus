project(Zacas_Test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)

add_executable(Amocas_test Amocas_test.cpp)
target_link_libraries(Amocas_test pegasussim)

pegasus_named_test(Amocas_test_run Amocas_test)
