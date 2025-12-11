project(PegasusRegister_Test)

add_executable(PegasusRegister_test Register_test.cpp)
target_link_libraries(PegasusRegister_test pegasussim)

pegasus_named_test(Register_test_run PegasusRegister_test)
pegasus_copy(PegasusRegister_test *.json)
pegasus_copy(PegasusRegister_test ../../arch/rva23/rv64/gen/reg_*.json)

add_subdirectory(macros)
