project(Atlas)

add_library(atlascore
    STATIC
    ActionGroup.cpp
    AtlasState.cpp
    ActionTags.cpp
    Fetch.cpp
    Translate.cpp
    Execute.cpp
    Exception.cpp
    AtlasExtractor.cpp
    AtlasInst.cpp
    PageTableWalker.cpp
    observers/Observer.cpp
    observers/CoSimObserver.cpp
    observers/InstructionLogger.cpp
)

target_include_directories(atlascore PUBLIC ${CMAKE_BINARY_DIR})
add_dependencies(atlascore AutogenArchFiles)

set(INST_HANDLERS_SCRIPT   "${SIM_BASE}/scripts/GenInstructionJSON.py")
set(INST_HANDLERS_RV64_DIR "${CMAKE_BINARY_DIR}/core/inst_handlers/rv64")
set(INST_HANDLERS_RV32_DIR "${CMAKE_BINARY_DIR}/core/inst_handlers/rv32")

# TODO: Get support rv64/rv32 ISA string from a variable
if(NOT EXISTS ${INST_HANDLERS_RV64_DIR})
  execute_process(
    COMMAND python3 ${INST_HANDLERS_SCRIPT} rv64imafd_Zicsr_Zifencei
  )
endif()

if(NOT EXISTS ${INST_HANDLERS_RV32_DIR})
  execute_process(
    COMMAND python3 ${INST_HANDLERS_SCRIPT} rv32imafd_Zicsr_Zifencei
  )
endif()

add_dependencies(atlascore atlasinsts)
