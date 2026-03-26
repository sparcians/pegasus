project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(smrmni
    OBJECT
    ${SOURCES}
)
add_dependencies(smrmni AutogenArchFiles)
target_link_libraries(smrmni PUBLIC pegasuslibs)
