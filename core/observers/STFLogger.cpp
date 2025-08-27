#include "STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/PegasusState.hpp"
#include "core/VecElements.hpp"

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

        stf_writer_.addTraceInfo(stf::TraceInfoRecord(stf::STF_GEN::STF_TRANSACTION_EXAMPLE, 0, 0,
                                                      0, "Trace from Pegasus"));

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

        // STF_VLEN_CONFIG 

        stf_writer_.setVLen(state->getVectorConfig()->getVLEN());
        stf_writer_.setISA(stf::ISA::RISCV);
        stf_writer_.setHeaderPC(inital_pc);
        stf_writer_.finalizeHeader();
        recordRegState_(state);
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
            if (reg_type == RegType::INTEGER) {
                return stf::Registers::STF_REG_TYPE::INTEGER;
            }
            else if (reg_type == RegType::FLOATING_POINT) {
                return stf::Registers::STF_REG_TYPE::FLOATING_POINT;
            }
            else if (reg_type == RegType::VECTOR) {
                return stf::Registers::STF_REG_TYPE::VECTOR;
            }
            else if (reg_type == RegType::CSR) {
                return stf::Registers::STF_REG_TYPE::CSR;
            }
            else {
                sparta_assert(false, "Invalid register type!");
            }
        };

        for (const auto & src_reg : src_regs_)
        {
            const auto stf_reg_type = get_stf_reg_type(src_reg.reg_id.reg_type);
            if (src_reg.reg_id.reg_type != RegType::VECTOR)
            {
                stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num, stf_reg_type,
                                                  stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                                  src_reg.reg_value.getValue<uint64_t>());
            }
            else
            {
                sparta_assert(false, "STF trace generation does not support vector yet!");
            }
        }

        for (const auto & [csr_num, csr_read] : csr_reads_)
        {
            stf_writer_ << stf::InstRegRecord(
                csr_num, stf::Registers::STF_REG_TYPE::CSR,
                stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE, csr_read.getRegValue<uint32_t>());
        }

        for (const auto & [csr_num, csr_write] : csr_writes_)
        {
            stf_writer_ << stf::InstRegRecord(
                csr_num, stf::Registers::STF_REG_TYPE::CSR,
                stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST, csr_write.getRegValue<uint32_t>());
        }

        for (const auto & dst_reg : dst_regs_)
        {
            const auto stf_reg_type = get_stf_reg_type(dst_reg.reg_id.reg_type);
            if (dst_reg.reg_id.reg_type != RegType::VECTOR)
            {
                stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num, stf_reg_type,
                                                  stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                                  readScalarRegister_<uint64_t>(state, dst_reg.reg_id));
            }
            else
            {
                sparta_assert(false, "STF trace generation does not support vector yet!");
            }
        }

        bool invalid_opcode = false;

        if (fault_cause_.isValid())
        {
            switch (fault_cause_.getValue())
            {
                case FaultCause::INST_ADDR_MISALIGNED:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INST_ADDR_MISALIGN,
                                                    READ_CSR_REG<uint64_t>(state, MEPC));
                    invalid_opcode = true;
                    break;
                case FaultCause::INST_ACCESS:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INST_ADDR_FAULT,
                                                    READ_CSR_REG<uint64_t>(state, MEPC));
                    invalid_opcode = true;
                    break;
                case FaultCause::INST_PAGE_FAULT:
                    stf_writer_ << stf::EventRecord(
                        stf::EventRecord::TYPE::INST_PAGE_FAULT,
                        {READ_CSR_REG<uint64_t>(state, MEPC), state->getXlen()});
                    invalid_opcode = true;
                    break;
                case FaultCause::LOAD_ADDR_MISALIGNED:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::LOAD_ADDR_MISALIGN,
                                                    {READ_CSR_REG<uint64_t>(state, MEPC),
                                                     state->getXlen(),
                                                     READ_CSR_REG<uint64_t>(state, MTVAL)});
                    break;
                case FaultCause::LOAD_ACCESS:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::LOAD_ACCESS_FAULT,
                                                    {READ_CSR_REG<uint64_t>(state, MEPC),
                                                     state->getXlen(),
                                                     READ_CSR_REG<uint64_t>(state, MTVAL)});
                    break;
                case FaultCause::STORE_AMO_ADDR_MISALIGNED:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::STORE_ADDR_MISALIGN,
                                                    {READ_CSR_REG<uint64_t>(state, MEPC),
                                                     state->getXlen(),
                                                     READ_CSR_REG<uint64_t>(state, MTVAL)});
                    break;
                case FaultCause::STORE_AMO_ACCESS:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::STORE_ACCESS_FAULT,
                                                    {READ_CSR_REG<uint64_t>(state, MEPC),
                                                     state->getXlen(),
                                                     READ_CSR_REG<uint64_t>(state, MTVAL)});
                    break;
                case FaultCause::LOAD_PAGE_FAULT:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::LOAD_PAGE_FAULT,
                                                    {READ_CSR_REG<uint64_t>(state, MEPC),
                                                     state->getXlen(),
                                                     READ_CSR_REG<uint64_t>(state, MTVAL)});
                    break;
                case FaultCause::STORE_AMO_PAGE_FAULT:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::STORE_PAGE_FAULT,
                                                    {READ_CSR_REG<uint64_t>(state, MEPC),
                                                     state->getXlen(),
                                                     READ_CSR_REG<uint64_t>(state, MTVAL)});
                    return; // tied to invalid opcode
                case FaultCause::ILLEGAL_INST:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::ILLEGAL_INST,
                                                    {READ_CSR_REG<uint64_t>(state, MEPC),
                                                     READ_CSR_REG<uint64_t>(state, MTVAL),
                                                     state->getXlen()});
                    invalid_opcode = true;
                    break;
                case FaultCause::BREAKPOINT:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::BREAKPOINT,
                                                    READ_CSR_REG<uint64_t>(state, MEPC));
                    break;
                case FaultCause::USER_ECALL:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::USER_ECALL,
                                                    READ_INT_REG<uint64_t>(state, 17));
                    break;
                case FaultCause::SUPERVISOR_ECALL:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::SUPERVISOR_ECALL,
                                                    READ_INT_REG<uint64_t>(state, 17));
                    break;
                case FaultCause::MACHINE_ECALL:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::MACHINE_ECALL,
                                                    READ_INT_REG<uint64_t>(state, 17));
                    break;
                default:
                    sparta_assert(false, "STFLogger: Unknown fault cause");
            }
            stf_writer_ << stf::EventPCTargetRecord(READ_CSR_REG<uint64_t>(state, MTVEC));
        }
        else if (interrupt_cause_.isValid())
        {
            switch (interrupt_cause_.getValue())
            {
                case InterruptCause::SUPERVISOR_SOFTWARE:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_SUPERVISOR_SOFTWARE,
                                                    {0});
                    break;
                case InterruptCause::MACHINE_SOFTWARE:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_MACHINE_SOFTWARE,
                                                    {0});
                    break;
                case InterruptCause::SUPERVISOR_TIMER:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_SUPERVISOR_TIMER,
                                                    {0});
                    break;
                case InterruptCause::MACHINE_TIMER:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_MACHINE_TIMER, {0});
                    break;
                case InterruptCause::SUPERVISOR_EXTERNAL:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_USER_EXT, {0});
                    break;
                case InterruptCause::MACHINE_EXTERNAL:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_MACHINE_EXT, {0});
                    break;
                case InterruptCause::COUNTER_OVERFLOW:
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::INT_USER_SOFTWARE, {0});
                    break;
                default:
                    sparta_assert(false, "STFLogger: Unknown interrupt");
            }
            stf_writer_ << stf::EventPCTargetRecord(READ_CSR_REG<uint64_t>(state, MTVEC));
        }
        else if (state->getNextPc()
                 != state->getPrevPc() + state->getCurrentInst()->getOpcodeSize())
        {
            stf_writer_ << stf::InstPCTargetRecord(state->getNextPc());
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

    void STFLogger::recordRegState_(PegasusState* state)
    {
        // Recording int registers
        for (uint64_t i = 0; i < 32; ++i)
        {
            stf_writer_ << stf::InstRegRecord(i, stf::Registers::STF_REG_TYPE::INTEGER,
                                              stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                              READ_INT_REG<uint64_t>(state, i));
        }
        // Recording fp registers
        for (uint64_t i = 0; i < state->getFpRegisterSet()->getNumRegisters(); ++i)
        {
            stf_writer_ << stf::InstRegRecord(i, stf::Registers::STF_REG_TYPE::FLOATING_POINT,
                                              stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                              READ_FP_REG<uint64_t>(state, i));
        }
        // Recording csr Registers
        auto csr_rset = state->getCsrRegisterSet();
        for (size_t i = 0; i < csr_rset->getNumRegisters(); ++i)
        {
            if (auto reg = csr_rset->getRegister(i))
            {
                stf_writer_ << stf::InstRegRecord(i, stf::Registers::STF_REG_TYPE::CSR,
                                                  stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                                  reg->dmiRead<uint64_t>());
            }
        }
    }
} // namespace pegasus
