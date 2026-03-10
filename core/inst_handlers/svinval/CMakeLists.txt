project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvsvinval
    OBJECT
    ${SOURCES}
)
add_dependencies(rvsvinval AutogenArchFiles)
target_link_libraries(rvsvinval PUBLIC pegasuslibs)
