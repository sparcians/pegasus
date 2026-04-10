project(STF_Test)

# This line will make sure pegasus is built before running the tests
pegasus_regress (pegasus)

add_executable(STF_test STF_test.cpp)
target_link_libraries(STF_test stf)

pegasus_named_test(STF_test_run STF_test)
