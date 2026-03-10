project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzfbfmin
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzfbfmin AutogenArchFiles)
target_link_libraries(rvzfbfmin PUBLIC pegasuslibs)
