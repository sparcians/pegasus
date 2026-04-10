project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzawrs
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzawrs AutogenArchFiles)
target_link_libraries(rvzawrs PUBLIC pegasuslibs)
