project(STF_Test)

# This line will make sure atlas is built before running the tests
atlas_regress (atlas)

add_executable(STF_test STF_test.cpp)
target_link_libraries(STF_test ${STF_LINK_LIBS})

atlas_named_test(STF_test_run STF_test)
