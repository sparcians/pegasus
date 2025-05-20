project(Actions_Test)

add_executable(Actions_test Actions_test.cpp)
target_link_libraries(Actions_test atlascore atlasinsts softfloat ${STF_LINK_LIBS} ${SPARTA_LIBS} mavis boost_json)

atlas_named_test(Actions_test_run Actions_test)
