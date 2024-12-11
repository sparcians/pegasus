project(AtlasRegister_Test)

add_executable(AtlasRegister_test Register_test.cpp)
target_link_libraries(AtlasRegister_test atlascore atlasinsts ${SPARTA_LIBS} mavis)

sparta_named_test(Register_test_run AtlasRegister_test)
sparta_copy(AtlasRegister_test *.json)
sparta_copy(AtlasRegister_test ../../arch/rv64/reg_*.json)

add_subdirectory(macros)
