project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzicfiss
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzicfiss AutogenArchFiles)
target_link_libraries(rvzicfiss PUBLIC pegasuslibs)
