project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzknd
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzknd AutogenArchFiles)
target_link_libraries(rvzknd PUBLIC pegasuslibs)