project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzabha
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzabha AutogenArchFiles)
target_link_libraries(rvzabha pegasuslibs)
