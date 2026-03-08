project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzvknh
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzvknh AutogenArchFiles)
target_link_libraries(rvzvknh PUBLIC pegasuslibs)
