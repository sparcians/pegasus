project(Atlas)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzba
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzba AutogenArchFiles)