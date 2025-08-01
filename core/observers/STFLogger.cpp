#include "STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/PegasusState.hpp"

namespace pegasus
{
    STFLogger::STFLogger(const uint32_t reg_width, uint64_t inital_pc, const std::string & filename,
                         PegasusState* state) :
        Observer((reg_width == 32) ? ObserverMode::RV32 : ObserverMode::RV64)
    {
        if (filename.find(".stf", filename.length() - 5)
            || filename.find(".zstf", filename.length() - 6)
            || filename.find(".STF", filename.length() - 5)
            || filename.find(".ZSTF", filename.length() - 6))
        {
            std::cout << "STF Trace filename inputted does not end with .stf or .zstf, appending "
                         ".zstf to the filename."
                      << std::endl;
            stf_writer_.open(filename + ".zstf");
        }
        else
        {
            stf_writer_.open(filename);
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

        stf_writer_.setISA(stf::ISA::RISCV);
        stf_writer_.setHeaderPC(inital_pc);
        stf_writer_.finalizeHeader();
        recordRegState_(state);
    }

    void STFLogger::postExecute_(PegasusState* state)
    {
        if (state->getNextPc() != state->getPrevPc() + state->getCurrentInst()->getOpcodeSize())
        {
            stf_writer_ << stf::InstPCTargetRecord(state->getNextPc());
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

        for (const auto & src_reg : src_regs_)
        {
            switch(src_reg.reg_id.reg_type){
                case RegType::INTEGER:
                    stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::INTEGER,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    READ_INT_REG<uint64_t>(state, src_reg.reg_id.reg_num));
                                    break;
                case RegType::FLOATING_POINT:
                    stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::FLOATING_POINT,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    READ_FP_REG<uint64_t>(state, src_reg.reg_id.reg_num));
                                    break;
                case RegType::CSR:
                    stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::CSR,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    READ_CSR_REG<uint64_t>(state, src_reg.reg_id.reg_num));
                                    break;
                default:
                    sparta_assert(false, "Invalid register type!");
            }
        }
        
        for (const auto & dst_reg : dst_regs_)
        {
            switch(dst_reg.reg_id.reg_type){
                case RegType::INTEGER:
                    stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::INTEGER,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    READ_INT_REG<uint64_t>(state, dst_reg.reg_id.reg_num));// dst_reg.reg_value
                                    break;
                case RegType::FLOATING_POINT:
                    stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::FLOATING_POINT,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    READ_FP_REG<uint64_t>(state, dst_reg.reg_id.reg_num));
                                    break;
                case RegType::CSR:
                    stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::CSR,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    READ_CSR_REG<uint64_t>(state, dst_reg.reg_id.reg_num));
                                    break;
                default:
                    sparta_assert(false, "Invalid register type!");
            }
        }

        if (fault_cause_.isValid() || interrupt_cause_.isValid()) { return; } // TODO: Add support for exceptions
        
        if (state->getCurrentInst()->getOpcodeSize() == 2)
        {
            stf_writer_ << stf::InstOpcode16Record(state->getCurrentInst()->getOpcode());
        }
        else
        {
            stf_writer_ << stf::InstOpcode32Record(state->getCurrentInst()->getOpcode());
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