project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzvkg
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzvkg AutogenArchFiles)
target_link_libraries(rvzvkg PUBLIC pegasuslibs)
