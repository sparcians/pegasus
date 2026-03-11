project(PegasusCoSim)

add_library(pegasuscosimlib
    STATIC
    PegasusCoSim.cpp
    CoSimEventReplayer.cpp
    CoSimEventPipeline.cpp
)

find_package(Boost REQUIRED COMPONENTS serialization)

target_link_libraries(pegasuscosimlib
    pegasussim
    Boost::serialization
)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/arch          ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../mavis/json ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../core/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

install(FILES ${PROJECT_BINARY_DIR}/libpegasuscosimlib.a DESTINATION lib)
install(FILES PegasusCoSim.hpp DESTINATION include/pegasus/cosim)
install(FILES CoSimEventReplayer.hpp DESTINATION include/pegasus/cosim)
install(FILES Event.hpp DESTINATION include/pegasus/cosim)
install(FILES CoSimApi.hpp DESTINATION include/pegasus/cosim)
install(FILES EventAccessor.hpp DESTINATION include/pegasus/cosim)
install(FILES MemoryInterface.hpp DESTINATION include/pegasus/cosim)
