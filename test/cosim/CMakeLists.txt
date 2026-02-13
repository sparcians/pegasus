macro(pegasus_add_cosim_test_executable target)
    add_executable (${target} ${ARGN})
    target_link_libraries(${target} PUBLIC pegasuscosimlib)
    target_include_directories(${target} PUBLIC ${SIM_BASE}/test/cosim)
endmacro()

macro(cosim_named_test name target)
    pegasus_named_test(${name} ${target} ${ARGN})
endmacro()

add_subdirectory(cosim_workload)
