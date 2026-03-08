project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzicbom
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzicbom AutogenArchFiles)
target_link_libraries(rvzicbom PUBLIC pegasuslibs)
