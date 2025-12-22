set(DEPENDS_REGISTER_PYTHON_SCRIPTS
  ${SIM_BASE}/scripts/GenRISCVRegisterDefinitions.py
  ${SIM_BASE}/scripts/RV32_CSR.py
  ${SIM_BASE}/scripts/RV64_CSR.py
  ${SIM_BASE}/scripts/REG_CONSTS.py
)

file(GLOB DEPENDS_INST_PYTHON_FILES ${SIM_BASE}/scripts/insts/RV*.py)
list(APPEND DEPENDS_INST_PYTHON_FILES
  ${SIM_BASE}/scripts/GenInstructionJSON.py
)

set(AUTOGEN_CSR_HEADERS
  ${SIM_BASE}/include/gen/CSRNums.hpp
  ${SIM_BASE}/include/gen/CSRBitMasks32.hpp
  ${SIM_BASE}/include/gen/CSRFieldIdxs32.hpp
  ${SIM_BASE}/include/gen/CSRBitMasks64.hpp
  ${SIM_BASE}/include/gen/CSRFieldIdxs64.hpp
)

set(AUTOGEN_SUPPORTED_ISA_HEADER
  ${SIM_BASE}/arch/gen/supportedISA.hpp
)

add_custom_target(AutogenArchFiles ALL
  SOURCES ${DEPENDS_REGISTER_PYTHON_SCRIPTS} ${DEPENDS_INST_PYTHON_FILES}
  DEPENDS ${AUTOGEN_CSR_HEADERS} ${AUTOGEN_SUPPORTED_ISA_HEADER}
)

# Generate register definition JSONs for each arch
add_custom_command(
  TARGET AutogenArchFiles
  COMMAND python3 ${SIM_BASE}/scripts/GenRISCVRegisterDefinitions.py
  DEPENDS ${DEPENDS_REGISTER_PYTHON_FILES}
)

# Generate instruction definition JSONs for rv32 and rv64 each arch
add_custom_command(
  TARGET AutogenArchFiles
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py rv32
  DEPENDS ${DEPENDS_INST_PYTHON_FILES}
)

add_custom_command(
  TARGET AutogenArchFiles
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py rv64
  DEPENDS ${DEPENDS_INST_PYTHON_FILES}
)

# Generate CSR headers
add_custom_command(
  OUTPUT ${AUTOGEN_CSR_HEADERS}
  COMMAND python3 ${SIM_BASE}/scripts/GenRISCVRegisterDefinitions.py --gen-csr-headers
  DEPENDS ${DEPENDS_REGISTER_PYTHON_SCRIPTS}
)

# Generate supported ISA header
add_custom_command(
  OUTPUT ${AUTOGEN_SUPPORTED_ISA_HEADER}
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py --gen-supported-isa-header
  DEPENDS ${DEPENDS_INST_PYTHON_FILES}
)

# Install
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rva23.yaml DESTINATION include/pegasus/arch)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rva23 DESTINATION include/pegasus/arch)
