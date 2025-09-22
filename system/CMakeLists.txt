project(Pegasus)

add_library(pegasussys
    PegasusSystem.cpp
    SimpleUART.cpp
    MagicMemory.cpp
    SystemCallEmulator.cpp
)
target_link_libraries(pegasussys PUBLIC pegasuslibs)

add_dependencies(pegasussys AutogenArchFiles)
