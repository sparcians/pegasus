project(Atlas)

add_subdirectory(rv64)

add_library(atlasinsts)
target_link_libraries(atlasinsts
    rvi
    rvm
    rva
    rvf
    rvd
    rvzicsr
    rvzifencei
)
add_dependencies(atlasinsts AutogenArchFiles)
