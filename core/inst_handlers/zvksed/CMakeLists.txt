project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzvksed
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzvksed AutogenArchFiles)
target_link_libraries(rvzvksed PUBLIC pegasuslibs)
