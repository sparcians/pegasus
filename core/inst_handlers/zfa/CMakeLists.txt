project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzfa
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzfa AutogenArchFiles)
target_link_libraries(rvzfa PUBLIC pegasuslibs)
