project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzicbop
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzicbop AutogenArchFiles)
target_link_libraries(rvzicbop PUBLIC pegasuslibs)
