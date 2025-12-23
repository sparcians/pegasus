project(Pegasus)

add_library(pegasussim
    PegasusSim.cpp
)

add_dependencies(pegasussim AutogenArchFiles)
target_link_libraries(pegasussim
    pegasuscore
    pegasusinsts
    pegasussys
    softfloat
    ${SPARTA_LIBS}
    ${STF_LINK_LIBS}
    mavis Boost::json
)

# Pegasus simulator
add_executable(pegasus pegasus.cpp)

target_link_libraries(pegasus pegasussim)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../arch                    ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../mavis/json              ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)

install(TARGETS pegasus DESTINATION bin)
install(FILES ${PROJECT_BINARY_DIR}/libpegasussim.a DESTINATION lib)
