set(AUTOGEN_RV32_REG_JSON_FILES
  ${SIM_BASE}/arch/rv32/gen/reg_csr.json
  ${SIM_BASE}/arch/rv32/gen/reg_fp.json
  ${SIM_BASE}/arch/rv32/gen/reg_int.json
  ${SIM_BASE}/arch/rv32/gen/reg_vec128.json
  ${SIM_BASE}/arch/rv32/gen/reg_vec256.json
  ${SIM_BASE}/arch/rv32/gen/reg_vec512.json
  ${SIM_BASE}/arch/rv32/gen/reg_vec1024.json
  ${SIM_BASE}/arch/rv32/gen/reg_vec2048.json
)

set(AUTOGEN_RV64_REG_JSON_FILES
  ${SIM_BASE}/arch/rv64/gen/reg_csr.json
  ${SIM_BASE}/arch/rv64/gen/reg_fp.json
  ${SIM_BASE}/arch/rv64/gen/reg_int.json
  ${SIM_BASE}/arch/rv64/gen/reg_vec128.json
  ${SIM_BASE}/arch/rv64/gen/reg_vec256.json
  ${SIM_BASE}/arch/rv64/gen/reg_vec512.json
  ${SIM_BASE}/arch/rv64/gen/reg_vec1024.json
  ${SIM_BASE}/arch/rv64/gen/reg_vec2048.json
)

set(AUTOGEN_CSR_HEADERS
  ${SIM_BASE}/include/gen/CSRNums.hpp
  ${SIM_BASE}/include/gen/CSRBitMasks32.hpp
  ${SIM_BASE}/include/gen/CSRFieldIdxs32.hpp
  ${SIM_BASE}/include/gen/CSRBitMasks64.hpp
  ${SIM_BASE}/include/gen/CSRFieldIdxs64.hpp
)

set(AUTOGEN_REGISTER_FILES
  ${AUTOGEN_RV32_REG_JSON_FILES}
  ${AUTOGEN_RV64_REG_JSON_FILES}
  ${AUTOGEN_CSR_HEADERS}
)

set(DEPENDS_REGISTER_FILES
  ${SIM_BASE}/scripts/GenRISCVRegisterDefinitions.py
  ${SIM_BASE}/scripts/RV32_CSR.py
  ${SIM_BASE}/scripts/RV64_CSR.py
  ${SIM_BASE}/scripts/REG_CONSTS.py
)

add_custom_command(
  OUTPUT ${AUTOGEN_REGISTER_FILES}
  COMMAND python3 ${SIM_BASE}/scripts/GenRISCVRegisterDefinitions.py
  DEPENDS ${DEPENDS_REGISTER_FILES}
)

set(AUTOGEN_RV32_INST_JSON_FILES
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32i.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32m.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32a.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32f.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32d.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zicsr.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zifencei.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zicond.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zba.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zbb.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zbc.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zbs.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zcmp.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zcmt.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zilsd.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zabha.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zihintntl.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zihintpause.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zbkb.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zfa.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zve32f.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zve32x.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zve64d.json
  ${SIM_BASE}/arch/rv32/gen/pegasus_uarch_rv32zve64x.json
)

set(AUTOGEN_RV64_INST_JSON_FILES
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64i.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64m.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64a.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64f.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64d.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zicsr.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zifencei.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zicond.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zba.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zbb.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zbc.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zbs.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zcmp.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zcmt.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zilsd.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zabha.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zihintntl.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zihintpause.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zbkb.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zfa.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zve32f.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zve32x.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zve64d.json
  ${SIM_BASE}/arch/rv64/gen/pegasus_uarch_rv64zve64x.json
)

set(AUTOGEN_INST_FILES
  ${AUTOGEN_RV32_INST_JSON_FILES}
  ${AUTOGEN_RV64_INST_JSON_FILES}
)

file(GLOB DEPENDS_INST_JSON_FILES ${SIM_BASE}/scripts/insts/RV*.py)

add_custom_command(
  OUTPUT ${AUTOGEN_RV32_INST_JSON_FILES}
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py rv32
  DEPENDS ${DEPENDS_INST_JSON_FILES}
)

add_custom_command(
  OUTPUT ${AUTOGEN_RV64_INST_JSON_FILES}
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py rv64
  DEPENDS ${DEPENDS_INST_JSON_FILES}
)

# Supported ISA header
set(AUTOGEN_SUPPORTED_ISA_HEADER
  ${SIM_BASE}/arch/gen/supportedISA.hpp
)

set(DEPENDS_SUPPORTED_ISA_HEADER
  ${DEPENDS_INST_JSON_FILES}
  ${SIM_BASE}/scripts/GenInstructionJSON.py
)

add_custom_command(
  OUTPUT ${AUTOGEN_SUPPORTED_ISA_HEADER}
  COMMAND python3 ${SIM_BASE}/scripts/GenInstructionJSON.py --gen-supported-isa-header
  DEPENDS ${DEPENDS_SUPPORTED_ISA_HEADER}
)


add_custom_target(AutogenArchFiles ALL DEPENDS ${AUTOGEN_REGISTER_FILES} ${AUTOGEN_INST_FILES} ${AUTOGEN_SUPPORTED_ISA_HEADER})
