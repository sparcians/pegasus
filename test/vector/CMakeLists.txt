project(pegasus_test)

# This line will make sure pegasus is built before running the tests
pegasus_regress (pegasus)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/workloads                      ${CMAKE_CURRENT_BINARY_DIR}/workloads SYMBOLIC)

# Tests
add_subdirectory(vcs)
add_subdirectory(via)
add_subdirectory(vls)
add_subdirectory(vm)
add_subdirectory(vro)
