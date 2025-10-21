project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzcmt
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzcmt AutogenArchFiles)
target_link_libraries(rvzcmt PUBLIC pegasuslibs)
