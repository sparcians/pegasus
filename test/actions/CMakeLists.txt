project(Actions_Test)

add_executable(Actions_test Actions_test.cpp)
target_link_libraries(Actions_test atlascore atlasinsts ${SPARTA} mavis)

sparta_named_test(Actions_test_run Actions_test)
