project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzvkned
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzvkned AutogenArchFiles)
target_link_libraries(rvzvkned PUBLIC pegasuslibs)
