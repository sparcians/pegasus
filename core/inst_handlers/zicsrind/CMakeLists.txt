project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzicsrind
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzicsrind AutogenArchFiles)
target_link_libraries(rvzicsrind PUBLIC pegasuslibs)
