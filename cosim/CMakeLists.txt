project(PegasusCoSim)

add_library(pegasuscosimlib
    STATIC
    PegasusCoSim.cpp
    CoSimPipeline.cpp
)

find_package(Boost REQUIRED COMPONENTS serialization)

target_link_libraries(pegasuscosimlib
    pegasussim
    pegasuscore
    pegasusinsts
    softfloat
    pegasussys
    ${PEGASUS_LIBS}
    Boost::serialization
)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/arch          ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../mavis/json ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../core/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)
