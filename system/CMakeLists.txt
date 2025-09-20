project(Pegasus)

add_library(pegasussys
    STATIC
    PegasusSystem.cpp
    SimpleUART.cpp
    MagicMemory.cpp
    SystemCallEmulator.cpp
)
target_link_libraries(pegasussys pegasuslibs)

add_dependencies(pegasussys AutogenArchFiles)
