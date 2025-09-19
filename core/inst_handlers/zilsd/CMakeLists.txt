project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzilsd
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzilsd AutogenArchFiles)
target_link_libraries(rvzilsd pegasuslibs)
