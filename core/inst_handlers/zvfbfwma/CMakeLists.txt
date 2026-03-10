project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzvfbfwma
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzvfbfwma AutogenArchFiles)
target_link_libraries(rvzvfbfwma PUBLIC pegasuslibs)
