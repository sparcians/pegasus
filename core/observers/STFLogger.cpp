<<<<<<< HEAD
#include "core/observers/STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/AtlasState.hpp"

namespace atlas
{
    // CONSTRUCTORS

    // stf_enable -> user can enable and disable STF Trace Generation
    // width -> register width (32 or 64)
    // pc -> initial program counter
    // filename -> name of the file the trace will be written to
    STFLogger::STFLogger(const uint32_t reg_width, uint64_t inital_pc, const std::string & filename,
                         AtlasState* state) :
=======
#include "STFLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "core/PegasusState.hpp"

namespace pegasus
{
    STFLogger::STFLogger(const uint32_t reg_width, uint64_t inital_pc, const std::string & filename,
                         PegasusState* state) :
>>>>>>> upstream/main
        Observer((reg_width == 32) ? ObserverMode::RV32 : ObserverMode::RV64)
    {
        // set up version and stf generation type
        stf_writer_.open(filename);
        stf_writer_.addTraceInfo(stf::TraceInfoRecord(stf::STF_GEN::STF_TRANSACTION_EXAMPLE, 0, 0,
<<<<<<< HEAD
                                                       0, "Trace from Atlas"));

        if (reg_width == 32)
        {
            stf_writer_.setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV32);
        }
        else
=======
                                                      0, "Trace from Pegasus"));

        if (reg_width == 64)
>>>>>>> upstream/main
        {
            stf_writer_.setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV64);
            stf_writer_.setTraceFeature(stf::TRACE_FEATURES::STF_CONTAIN_RV64);
        }
<<<<<<< HEAD
=======
        else
        {
            stf_writer_.setHeaderIEM(stf::INST_IEM::STF_INST_IEM_RV32);
        }
>>>>>>> upstream/main

        stf_writer_.setISA(stf::ISA::RISCV);
        stf_writer_.setHeaderPC(inital_pc);
        stf_writer_.finalizeHeader();
<<<<<<< HEAD
        recordRegState_(state); //record inital state of registers
    }

    // METHODS
    void STFLogger::postExecute_(AtlasState* state)
    {
        if (fault_cause_.isValid() || interrupt_cause_.isValid())
        {
            // Do not write instruction record if there is a fault or interrupt
            return;
        } 
        for (const auto & dst_reg : dst_regs_)
        {
            switch(dst_reg.reg_id.reg_type){
                case RegType::INTEGER:
                    stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::INTEGER,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    dst_reg.reg_value);
                                    break;
                case RegType::FLOATING_POINT:
                    stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::FLOATING_POINT,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    dst_reg.reg_value);
                                    break;
                case RegType::VECTOR:
                    stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::VECTOR,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    dst_reg.reg_value);
                                    break;
                case RegType::CSR:
                    stf_writer_ << stf::InstRegRecord(dst_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::CSR,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                    dst_reg.reg_value);
                                    break;
                default:
                    sparta_assert(false, "Invalid register type!");
            }
        }

        if ((state->getCurrentInst()->getOpcode() & 0x7F) == 0x63 || (state->getCurrentInst()->getOpcode() & 0x7F) == 0x6F) // branch, jump
        {
            stf_writer_ << stf::InstPCTargetRecord(state->getNextPc());
        }

        // write opcode record
        if (state->getCurrentInst()->getOpcodeSize() == 2)
        {
            stf_writer_ << stf::InstOpcode16Record(state->getCurrentInst()->getOpcode());
        }
        else
        {
            stf_writer_ << stf::InstOpcode32Record(state->getCurrentInst()->getOpcode());
        }
    }

    void STFLogger::preExecute_(AtlasState* state) {
        for (const auto & src_reg : src_regs_)
        {
            switch(src_reg.reg_id.reg_type){
                case RegType::INTEGER:
                    stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::INTEGER,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                    src_reg.reg_value);
                                    break;
                case RegType::FLOATING_POINT:
                    stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::FLOATING_POINT,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                    src_reg.reg_value);
                                    break;
                case RegType::VECTOR:
                    stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::VECTOR,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                    src_reg.reg_value);
                                    break;
                case RegType::CSR:
                    stf_writer_ << stf::InstRegRecord(src_reg.reg_id.reg_num,
                                    stf::Registers::STF_REG_TYPE::CSR,
                                    stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                    src_reg.reg_value);
                                    break;
                default:
                    sparta_assert(false, "Invalid register type!");
            }
        }

        (void) state;
    }

    void STFLogger::exceptionCodeRecord (AtlasState* state)
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



    void STFLogger::preException_(AtlasState* state) {
        if(false) //fault_cause_.isValid()
        {
            std::vector<uint64_t> content_data = {0x0000000080000094};
            //need event number, type of event, number of metadata fields?? 
            //getValue() gives exception code
            
            /*stf_writer_ << stf::InstRegRecord(0,
                                stf::Registers::STF_REG_TYPE::INTEGER,
                                stf::Registers::STF_REG_OPERAND_TYPE::REG_SOURCE,
                                READ_INT_REG<uint64_t>(state, 0));*/
            stf_writer_ << stf::EventRecord(stf::EventRecord::TYPE::ILLEGAL_INST, content_data);

            /*stf_writer_ << stf::InstRegRecord(0,
                                stf::Registers::STF_REG_TYPE::INTEGER,
                                stf::Registers::STF_REG_OPERAND_TYPE::REG_DEST,
                                READ_INT_REG<uint64_t>(state, 0));*/
            stf_writer_ << stf::EventPCTargetRecord(READ_CSR_REG<uint64_t>(state, MTVEC) & ~1);
        }
        else if (interrupt_cause_.isValid())
        { 
            
        }
        (void) state;
    }

    void STFLogger::recordRegState_(AtlasState* state) {
        //Recording int registers
        for (uint32_t i = 0; i < 32; ++i)
        {
            stf_writer_ << stf::InstRegRecord(i,
                                stf::Registers::STF_REG_TYPE::INTEGER,
                                stf::Registers::STF_REG_OPERAND_TYPE::REG_STATE,
                                READ_INT_REG<uint64_t>(state, i));

        }
        //Recording fp registers
        for (uint32_t i = 0; i < state->getFpRegisterSet()->getNumRegisters(); ++i)
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
} // namespace atlas
=======
        (void)state; // Unused parameter, but will be implemented when recording inital state of
                     // registers
        // recordRegState_(state); record inital state of registers
    }

    void STFLogger::writeInstruction_(const PegasusInst* inst)
    {
        if (fault_cause_.isValid() || interrupt_cause_.isValid())
        {
            return;
        }

        if (inst->getOpcodeSize() == 2)
        {
            stf_writer_ << stf::InstOpcode16Record(inst->getOpcode());
        }
        else
        {
            stf_writer_ << stf::InstOpcode32Record(inst->getOpcode());
        }
    }

    void STFLogger::postExecute_(PegasusState* state)
    {
        // write opcode record
        writeInstruction_(state->getCurrentInst().get());
    }

    void STFLogger::preExecute_(PegasusState* state) { (void)state; }

    void STFLogger::preException_(PegasusState* state) { (void)state; }

    void STFLogger::recordRegState_(PegasusState* state) { (void)state; }
} // namespace pegasus
>>>>>>> upstream/main
