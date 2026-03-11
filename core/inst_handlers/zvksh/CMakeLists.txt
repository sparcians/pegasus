project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzvksh
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzvksh AutogenArchFiles)
target_link_libraries(rvzvksh PUBLIC pegasuslibs)
