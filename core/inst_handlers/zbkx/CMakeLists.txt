project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzbkx
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzbkx AutogenArchFiles)
target_link_libraries(rvzbkx PUBLIC pegasuslibs)
