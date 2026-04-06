project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzvbc
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzvbc AutogenArchFiles)
target_link_libraries(rvzvbc PUBLIC pegasuslibs)