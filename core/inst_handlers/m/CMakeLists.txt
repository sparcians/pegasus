project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvm
    OBJECT
    ${SOURCES}
)
add_dependencies(rvm AutogenArchFiles)
target_link_libraries(rvm pegasuslibs)
