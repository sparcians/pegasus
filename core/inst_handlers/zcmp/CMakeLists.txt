project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzcmp
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzcmp AutogenArchFiles)
target_link_libraries(rvzcmp PUBLIC pegasuslibs)
