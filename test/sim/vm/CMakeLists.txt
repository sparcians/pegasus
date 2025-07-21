project(Vm_Test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

add_executable(Vm_test Vm_test.cpp)
<<<<<<< HEAD
target_link_libraries(Vm_test atlassim atlascore atlasinsts softfloat atlassys ${SPARTA_LIBS} mavis boost_json)

atlas_named_test(Vm_test_run Vm_test)
=======
target_link_libraries(Vm_test pegasussim pegasuscore pegasusinsts softfloat pegasussys ${PEGASUS_LIBS})

pegasus_named_test(Vm_test_run Vm_test)
>>>>>>> upstream/main
