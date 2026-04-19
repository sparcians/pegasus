project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvsmrmni
    OBJECT
    ${SOURCES}
)
add_dependencies(rvsmrmni AutogenArchFiles)
target_link_libraries(rvsmrmni PUBLIC pegasuslibs)
