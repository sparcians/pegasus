project(Pegasus)

add_library(pegasussim
    STATIC
    PegasusSim.cpp
)

add_dependencies(pegasussim AutogenArchFiles)

# Pegasus simulator
add_executable(pegasus pegasus.cpp)

<<<<<<< HEAD
target_link_libraries(atlas atlassim atlascore atlasinsts softfloat atlassys ${ATLAS_LIBS} ${STF_LINK_LIBS})
=======
target_link_libraries(pegasus pegasussim pegasuscore pegasusinsts softfloat pegasussys ${PEGASUS_LIBS})
>>>>>>> upstream/main

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../arch                    ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../mavis/json              ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
