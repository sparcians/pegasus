macro(cosim_named_test name target)
    pegasus_named_test(${name} ${target} ${ARGN})
    target_link_libraries(${target} PRIVATE pegasuscosimlib)
    target_include_directories(${target} PRIVATE ${SIM_BASE}/test/cosim)
endmacro()

add_subdirectory(cosim_workload)
