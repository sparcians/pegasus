project(Pegasus)
file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzbkb
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzbkb AutogenArchFiles)
target_link_libraries(rvzbkb PUBLIC pegasuslibs)
