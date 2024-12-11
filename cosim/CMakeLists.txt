project(AtlasCoSim)

add_library(atlascosimlib
    STATIC
    AtlasCoSim.cpp
)

target_link_libraries(atlascosimlib
    atlassim
    atlascore
    atlasinsts
    atlassys
    ${SPARTA_LIBS}
    mavis
)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/arch          ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../mavis/json ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../core/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)
