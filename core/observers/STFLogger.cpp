#include "STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/PegasusState.hpp"
#include "sparta/utils/LogUtils.hpp"

#define STF_DLOG(msg) SPARTA_LOG(state->getDebugLogger(), msg); // include STFLogger in debug log 

namespace pegasus
{
    STFLogger::STFLogger(const uint32_t reg_width, uint64_t inital_pc, const std::string & filename,
                         PegasusState* state) :
        Observer((reg_width == 32) ? ObserverMode::RV32 : ObserverMode::RV64)
    {
        // set up version and stf generation type
        stf_writer_.open(filename);
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

        // Is there a parameter for the user to enable page tables and physical memory tracing?
        stf_writer_.setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_PTE);

        stf_writer_.setISA(stf::ISA::RISCV);
        stf_writer_.setHeaderPC(inital_pc);
        stf_writer_.finalizeHeader();
        recordRegState_(state); //record inital state of registers
    }

    // METHODS
    void STFLogger::postExecute_(PegasusState* state)
    {
        STF_DLOG("Executing postExecute");
        if (fault_cause_.isValid() || interrupt_cause_.isValid())
        {
            std::vector<uint64_t> content;
            // Do not write instruction record if there is a fault or interrupt
            if(fault_cause_.isValid())
            {
                if (fault_cause_.getValue() == FaultCause::ILLEGAL_INST)
                {
                    stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::ILLEGAL_INST, {READ_CSR_REG<uint64_t>(state, MEPC), READ_CSR_REG<uint64_t>(state, MTVAL), stf::INST_IEM::STF_INST_IEM_RV64});
                    stf_writer_ << stf::EventPCTargetRecord(READ_CSR_REG<uint64_t>(state, MTVEC));
                }
                else if (false)
                {
                    switch(fault_cause_.getValue())
                    {
                        STF_DLOG("in postExecute_() with fault cause: " << fault_cause_.getValue());
                        case FaultCause::INST_ADDR_MISALIGNED:
                            break;
                        case FaultCause::INST_ACCESS: //INST_ADDR_FAULT WORK ON
                            break;
                        case FaultCause::ILLEGAL_INST:
                            stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::ILLEGAL_INST, READ_CSR_REG<uint64_t>(state, MTVAL));
                            stf_writer_ << stf::EventPCTargetRecord(READ_CSR_REG<uint64_t>(state, MTVEC));
                            break;
                        case FaultCause::BREAKPOINT:
                            break;
                        case FaultCause::LOAD_ADDR_MISALIGNED:
                            break;
                        case FaultCause::LOAD_ACCESS:
                        case FaultCause::STORE_AMO_ADDR_MISALIGNED: //STORE_ADDR_MISALIGN:
                        case FaultCause::STORE_AMO_ACCESS: //STORE_ACCESS_FAULT:
                        case FaultCause::USER_ECALL:
                        case FaultCause::SUPERVISOR_ECALL:
                        case FaultCause::MACHINE_ECALL:
                            break;
                        case FaultCause::INST_PAGE_FAULT:
                        case FaultCause::LOAD_PAGE_FAULT:
                        case FaultCause::STORE_AMO_PAGE_FAULT:
                        default: 
                            //sparta_assert(false, "Unhandled fault cause in exceptionCodeRecord");
                            std::cout << "Skip" << std::endl;
                    }
                }
            }
            else if (interrupt_cause_.isValid())
            { 
                
            }
        }
        else if (state->getNextPc() != state->getPrevPc() + state->getCurrentInst()->getOpcodeSize()) // branch and jumps
        {
            stf_writer_ << stf::InstPCTargetRecord(state->getNextPc());
        }

        STF_DLOG("FINIISHED");

        // What is the third parameter in InstMemAccessRecord (memory attributes)
        for (const auto & mem_write : mem_writes_)
        {
            stf_writer_ << stf::InstMemAccessRecord(mem_write.addr, mem_write.size, 0,
                                                   stf::INST_MEM_ACCESS::WRITE);
            // will I be accessign memory that is greater than 8 bytes?
            stf_writer_ << stf::InstMemContentRecord(mem_write.value);
        }

        for (const auto & mem_read : mem_reads_)
        {
            stf_writer_ << stf::InstMemAccessRecord(mem_read.addr, mem_read.size, 0,
                                                   stf::INST_MEM_ACCESS::READ);
            stf_writer_ << stf::InstMemContentRecord(mem_read.value);
        }

        if (state->getCurrentInst()->getOpcodeSize() == 2)
        {
            stf_writer_ << stf::InstOpcode16Record(state->getCurrentInst()->getOpcode());
        }
        else
        {
            stf_writer_ << stf::InstOpcode32Record(state->getCurrentInst()->getOpcode());
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
                case RegType::VECTOR:
                    stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::VECTOR,
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
                case RegType::VECTOR:
                    stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::VECTOR,
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

    }

    void STFLogger::exceptionCodeRecord (PegasusState* state)
    {
        switch (fault_cause_.getValue())
        {
            case FaultCause::INST_ADDR_MISALIGNED:
            case FaultCause::INST_ACCESS: //INST_ADDR_FAULT
            case FaultCause::ILLEGAL_INST:
                //stf_writer_ << stf::EventRecord(stf:;FaultCause::ILLEGAL_INST);
                break;
            case FaultCause::BREAKPOINT:
            case FaultCause::LOAD_ADDR_MISALIGNED:
            case FaultCause::LOAD_ACCESS:
            case FaultCause::STORE_AMO_ADDR_MISALIGNED: //STORE_ADDR_MISALIGN:
            case FaultCause::STORE_AMO_ACCESS: //STORE_ACCESS_FAULT:
            case FaultCause::USER_ECALL:
            case FaultCause::SUPERVISOR_ECALL:
            //case FaultCause::HYPERVISOR_ECALL:
            case FaultCause::MACHINE_ECALL:
            case FaultCause::INST_PAGE_FAULT:
            case FaultCause::LOAD_PAGE_FAULT:
            case FaultCause::STORE_AMO_PAGE_FAULT:
            /*case FaultCause::GUEST_INST_PAGE_FAULT:
            case FaultCause::GUEST_LOAD_PAGE_FAULT:
            //case FaultCause::VIRTUAL_INST:
            case FaultCause::GUEST_STORE_PAGE_FAULT:*/
            default: 
                sparta_assert(false, "Unhandled fault cause in exceptionCodeRecord");

        }

       (void) state;
    }

    void STFLogger::recordRegState_(PegasusState* state) {
        //Recording int registers
        for (uint64_t i = 0; i < 32; ++i)
        {
            stf_writer_ << stf::InstRegRecord(i,
                                stf::Registers::STF_REG_TYPE::INTEGER,
                                stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                READ_INT_REG<uint64_t>(state, i));

        }
        //Recording fp registers
        for (uint64_t i = 0; i < state->getFpRegisterSet()->getNumRegisters(); ++i)
        {
            stf_writer_ << stf::InstRegRecord(i,
                                stf::Registers::STF_REG_TYPE::FLOATING_POINT,
                                stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                READ_FP_REG<uint64_t>(state, i));

        }
        //Recording csr Registers
        auto csr_rset = state->getCsrRegisterSet();
        for (size_t i = 0; i < csr_rset->getNumRegisters(); ++i) {
            if (auto reg = csr_rset->getRegister(i)) {
                stf_writer_ << stf::InstRegRecord(i,
                            stf::Registers::STF_REG_TYPE::CSR,
                            stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                            reg->dmiRead<uint64_t>());
            }
        }
    }
} // namespace pegasus
