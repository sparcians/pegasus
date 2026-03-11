project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzicboz
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzicboz AutogenArchFiles)
target_link_libraries(rvzicboz PUBLIC pegasuslibs)
