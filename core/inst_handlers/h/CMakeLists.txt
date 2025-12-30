project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvh
    OBJECT
    ${SOURCES}
)
add_dependencies(rvh AutogenArchFiles)
target_link_libraries(rvh PUBLIC pegasuslibs)
