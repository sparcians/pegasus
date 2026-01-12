#include "STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/PegasusState.hpp"
#include "core/VecElements.hpp"
#include "include/gen/pegasus_version.hpp"
#include "core/PegasusCore.hpp"

namespace pegasus
{
    STFLogger::STFLogger(const uint32_t reg_width, uint64_t inital_pc, const std::string & filename,
                         PegasusState* state) :
        Observer((reg_width == 32) ? ObserverMode::RV32 : ObserverMode::RV64)
    {
        try
        {
            stf_writer_.open(filename);
        }
        catch (...)
        {
            std::cerr
                << "STF Filename formatted incorrectly: does the file have a STF file extension"
                << std::endl;
            throw;
        }

        const std::string pegasus_version =
            "Pegasus v" + MAJOR_VERSION + "." + MINOR_VERSION + "." + MINOR_MINOR_VERSION;
        stf_writer_.addTraceInfo(stf::TraceInfoRecord(
            stf::STF_GEN::STF_GEN_PEGASUS, std::stoul(MAJOR_VERSION), std::stoul(MINOR_VERSION),
            std::stoul(MINOR_MINOR_VERSION), pegasus_version));

        if (state->getXlen() == 32)
        {
            stf_writer_.setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV32);
        }
        else
        {
            stf_writer_.setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV64);
            stf_writer_.setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_RV64);
        }

        // TODO: Add support for PTE
        // stf_writer_.setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_PTE);

        const auto & isa = state->getCore()->getISAString();

        stf_writer_.setVLen(state->getVectorConfig()->getVLEN());
        stf_writer_.setISA(stf::ISA::RISCV);
        stf_writer_.setISAExtendedInfo(isa);
        stf_writer_.setHeaderPC(inital_pc);
        stf_writer_.finalizeHeader();
        if (state->getXlen() == 32)
        {
            recordRegState_<uint32_t>(state);
        }
        else
        {
            recordRegState_<uint64_t>(state);
        }
    }

    template <typename XLEN, typename F>
    void STFLogger::writeInstRegRecord_(PegasusState* state, F get_stf_reg_type)
    {
        for (const auto & src_reg : src_regs_)
        {
            const auto stf_reg_type = get_stf_reg_type(src_reg.reg_id.reg_type);
            if (src_reg.reg_id.reg_type != RegType::VECTOR)
            {
                stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num, stf_reg_type,
                                                  stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                                  src_reg.reg_value.getValue<XLEN>());
            }
            else
            {
                uint32_t encoded_lmul = state->getCurrentInst()->getVecConfig()->getLMUL();
                uint32_t reg_count = std::max(1u, encoded_lmul / 8); // In case of fractional LMUL.
                for (uint32_t i = 0; i < reg_count; ++i)
                {
                    uint32_t phys = src_reg.reg_id.reg_num + i;
                    stf_writer_ << stf::InstRegRecord(
                        phys, stf_reg_type, stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                        src_reg.lmul_values[i].getValueVector<uint64_t>());
                }
            }
        }

        for (const auto & [csr_num, csr_read] : csr_reads_)
        {
            stf_writer_ << stf::InstRegRecord(csr_num, stf::Registers::STF_REG_TYPE::CSR,
                                              stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                              csr_read.template getRegValue<XLEN>());
        }

        for (const auto & [csr_num, csr_write] : csr_writes_)
        {
            stf_writer_ << stf::InstRegRecord(csr_num, stf::Registers::STF_REG_TYPE::CSR,
                                              stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                              csr_write.template getRegValue<XLEN>());
        }

        for (const auto & dst_reg : dst_regs_)
        {
            const auto stf_reg_type = get_stf_reg_type(dst_reg.reg_id.reg_type);
            if (dst_reg.reg_id.reg_type != RegType::VECTOR)
            {
                stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num, stf_reg_type,
                                                  stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                                  readScalarRegister_<XLEN>(state, dst_reg.reg_id));
            }
            else
            {
                uint32_t encoded_lmul = state->getCurrentInst()->getVecConfig()->getLMUL();
                uint32_t reg_count = std::max(1u, encoded_lmul / 8); // In case of fractional LMUL.
                for (uint32_t i = 0; i < reg_count; ++i)
                {
                    uint32_t phys = dst_reg.reg_id.reg_num + i;
                    stf_writer_ << stf::InstRegRecord(
                        phys, stf_reg_type, stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                        readVectorRegister_(
                            state, RegId{RegType::VECTOR, phys, "V" + std::to_string(phys)}));
                }
            }
        }

        if (state->getCurrentInst()->getMavisOpcodeInfo()->isInstType(
                mavis::OpcodeInfo::InstructionTypes::VECTOR))
        {
            if (state->getCurrentInst()->isVectorInstMasked())
            {
                stf_writer_ << stf::InstRegRecord(
                    pegasus::V0, stf::Registers::STF_REG_TYPE::VECTOR,
                    stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                    readVectorRegister_(state, RegId{RegType::VECTOR, pegasus::V0, "V0"}));
            }

            stf_writer_ << stf::InstRegRecord(VL, stf::Registers::STF_REG_TYPE::CSR,
                                              stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                              READ_CSR_REG<XLEN>(state, VL));

            stf_writer_ << stf::InstRegRecord(VTYPE, stf::Registers::STF_REG_TYPE::CSR,
                                              stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                              READ_CSR_REG<XLEN>(state, VTYPE));
        }
    }

    template <typename XLEN>
    void STFLogger::writeEventRecord_(PegasusState* state, bool & invalid_opcode)
    {
        if (fault_cause_.isValid())
        {
            switch (fault_cause_.getValue())
            {
                case FaultCause::INST_ADDR_MISALIGNED:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::INST_ADDR_MISALIGN,
                        static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)));
                    invalid_opcode = true;
                    break;

                case FaultCause::INST_ACCESS:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::INST_ADDR_FAULT,
                        static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)));
                    invalid_opcode = true;
                    break;

                case FaultCause::INST_PAGE_FAULT:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::INST_PAGE_FAULT,
                        {static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)),
                         static_cast<XLEN>(state->getXlen())});
                    invalid_opcode = true;
                    break;

                case FaultCause::LOAD_ADDR_MISALIGNED:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::LOAD_ADDR_MISALIGN,
                        {static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)),
                         static_cast<XLEN>(state->getXlen()),
                         static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MTVAL))});
                    break;

                case FaultCause::LOAD_ACCESS:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::LOAD_ACCESS_FAULT,
                        {static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)),
                         static_cast<XLEN>(state->getXlen()),
                         static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MTVAL))});
                    break;

                case FaultCause::STORE_AMO_ADDR_MISALIGNED:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::STORE_ADDR_MISALIGN,
                        {static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)),
                         static_cast<XLEN>(state->getXlen()),
                         static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MTVAL))});
                    break;

                case FaultCause::STORE_AMO_ACCESS:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::STORE_ACCESS_FAULT,
                        {static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)),
                         static_cast<XLEN>(state->getXlen()),
                         static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MTVAL))});
                    break;

                case FaultCause::LOAD_PAGE_FAULT:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::LOAD_PAGE_FAULT,
                        {static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)),
                         static_cast<XLEN>(state->getXlen()),
                         static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MTVAL))});
                    break;

                case FaultCause::STORE_AMO_PAGE_FAULT:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::STORE_PAGE_FAULT,
                        {static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)),
                         static_cast<XLEN>(state->getXlen()),
                         static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MTVAL))});
                    return; // tied to invalid opcode

                case FaultCause::ILLEGAL_INST:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::ILLEGAL_INST,
                        {static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)),
                         static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MTVAL)),
                         static_cast<XLEN>(state->getXlen())});
                    invalid_opcode = true;
                    break;

                case FaultCause::BREAKPOINT:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::BREAKPOINT,
                        static_cast<XLEN>(READ_CSR_REG<XLEN>(state, MEPC)));
                    break;

                case FaultCause::USER_ECALL:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::USER_ECALL,
                        static_cast<XLEN>(READ_INT_REG<XLEN>(state, 17)));
                    break;

                case FaultCause::SUPERVISOR_ECALL:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::SUPERVISOR_ECALL,
                        static_cast<XLEN>(READ_INT_REG<XLEN>(state, 17)));
                    break;

                case FaultCause::MACHINE_ECALL:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::MACHINE_ECALL,
                        static_cast<XLEN>(READ_INT_REG<XLEN>(state, 17)));
                    break;

                default:
                    sparta_assert(false, "STFLogger: Unknown fault cause");
            }

            stf_writer_ << stf::EventPCTargetRecord(
                static_cast<uint64_t>(READ_CSR_REG<XLEN>(state, MTVEC)));
        }
        else if (interrupt_cause_.isValid())
        {
            switch (interrupt_cause_.getValue())
            {
                case InterruptCause::SUPERVISOR_SOFTWARE:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_SUPERVISOR_SOFTWARE,
                                                    {static_cast<XLEN>(0)});
                    break;

                case InterruptCause::MACHINE_SOFTWARE:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_MACHINE_SOFTWARE,
                                                    {static_cast<XLEN>(0)});
                    break;

                case InterruptCause::SUPERVISOR_TIMER:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_SUPERVISOR_TIMER,
                                                    {static_cast<XLEN>(0)});
                    break;

                case InterruptCause::MACHINE_TIMER:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_MACHINE_TIMER,
                                                    {static_cast<XLEN>(0)});
                    break;

                case InterruptCause::SUPERVISOR_EXTERNAL:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_USER_EXT,
                                                    {static_cast<XLEN>(0)});
                    break;

                case InterruptCause::MACHINE_EXTERNAL:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_MACHINE_EXT,
                                                    {static_cast<XLEN>(0)});
                    break;

                case InterruptCause::COUNTER_OVERFLOW:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_USER_SOFTWARE,
                                                    {static_cast<XLEN>(0)});
                    break;

                default:
                    sparta_assert(false, "STFLogger: Unknown interrupt");
            }

            stf_writer_ << stf::EventPCTargetRecord(
                static_cast<uint64_t>(READ_CSR_REG<XLEN>(state, MTVEC)));
        }
        else if (state->getCurrentInst()->isChangeOfFlowInst())
        {
            stf_writer_ << stf::InstPCTargetRecord(state->getNextPc());
        }
    }

    void STFLogger::postExecute_(PegasusState* state)
    {
        if (state->getCurrentInst() == nullptr)
        {
            return;
        }

        for (const auto & mem_write : mem_writes_)
        {
            stf_writer_ << stf::InstMemAccessRecord(mem_write.addr, mem_write.size, 0,
                                                    stf::INST_MEM_ACCESS::WRITE);
            stf_writer_ << stf::InstMemContentRecord(mem_write.value);
        }

        for (const auto & mem_read : mem_reads_)
        {
            stf_writer_ << stf::InstMemAccessRecord(mem_read.addr, mem_read.size, 0,
                                                    stf::INST_MEM_ACCESS::READ);
            stf_writer_ << stf::InstMemContentRecord(mem_read.value);
        }

        auto get_stf_reg_type = [](const RegType reg_type)
        {
            switch (reg_type)
            {
                case RegType::INTEGER:
                    return stf::Registers::STF_REG_TYPE::INTEGER;

                case RegType::FLOATING_POINT:
                    return stf::Registers::STF_REG_TYPE::FLOATING_POINT;

                case RegType::VECTOR:
                    return stf::Registers::STF_REG_TYPE::VECTOR;

                case RegType::CSR:
                    return stf::Registers::STF_REG_TYPE::CSR;

                default:
                    sparta_assert(false, "Invalid register type!");
                    // Added a dummy return to satisfy compiler (unreachable)
                    return stf::Registers::STF_REG_TYPE::INTEGER;
            }
        };

        bool invalid_opcode = false;

        if (state->getXlen() == 32)
        {
            writeInstRegRecord_<uint32_t>(state, get_stf_reg_type);
            writeEventRecord_<uint32_t>(state, invalid_opcode);
        }
        else
        {
            writeInstRegRecord_<uint64_t>(state, get_stf_reg_type);
            writeEventRecord_<uint64_t>(state, invalid_opcode);
        }

        uint64_t opcode = state->getCurrentInst()->getOpcode();

        if (invalid_opcode)
        {
            opcode = 0;
        }

        if (state->getCurrentInst()->getOpcodeSize() == 2)
        {
            stf_writer_ << stf::InstOpcode16Record(opcode);
        }
        else
        {
            stf_writer_ << stf::InstOpcode32Record(opcode);
        }
    }

    template <typename XLEN> void STFLogger::recordRegState_(PegasusState* state)
    {
        // Recording int registers
        for (uint64_t i = 0; i < 32; ++i)
        {
            stf_writer_ << stf::InstRegRecord(i, stf::Registers::STF_REG_TYPE::INTEGER,
                                              stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                              READ_INT_REG<XLEN>(state, i));
        }
        // Recording fp registers
        for (uint64_t i = 0; i < state->getFpRegisterSet()->getNumRegisters(); ++i)
        {
            stf_writer_ << stf::InstRegRecord(i, stf::Registers::STF_REG_TYPE::FLOATING_POINT,
                                              stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                              READ_FP_REG<XLEN>(state, i));
        }
        // Recording vector registers
        for (uint32_t i = 0; i < state->getVecRegisterSet()->getNumRegisters(); ++i)
        {
            stf_writer_ << stf::InstRegRecord(
                i, stf::Registers::STF_REG_TYPE::VECTOR,
                stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                readVectorRegister_(state, RegId{RegType::VECTOR, i, "V" + std::to_string(i)}));
        }
        // Recording csr Registers
        auto csr_rset = state->getCsrRegisterSet();
        for (size_t i = 0; i < csr_rset->getNumRegisters(); ++i)
        {
            if (auto reg = csr_rset->getRegister(i))
            {
                stf_writer_ << stf::InstRegRecord(i, stf::Registers::STF_REG_TYPE::CSR,
                                                  stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                                  reg->dmiRead<XLEN>());
            }
        }
    }
} // namespace pegasus
