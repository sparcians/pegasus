#include "core/observers/InstructionLogger.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"

#include "system/AtlasSystem.hpp"

#include "sparta/utils/LogUtils.hpp"

namespace atlas
{
#ifndef INSTLOG
#define INSTLOG(msg) SPARTA_LOG(inst_logger_, msg)
#endif

    template <typename XLEN>
    InstructionLogger<XLEN>::InstructionLogger(sparta::log::MessageSource & inst_logger) :
        inst_logger_(inst_logger)
    {
    }

    template class InstructionLogger<RV64>;
    template class InstructionLogger<RV32>;

    template <typename XLEN> ActionGroup* InstructionLogger<XLEN>::preExecute_(AtlasState* state)
    {
        pc_ = state->getPc();
        AtlasInstPtr inst = state->getCurrentInst();
        if (inst)
        {
            opcode_ = inst->getOpcode();

            // Get value of source registers
            if (inst->hasRs1())
            {
                const auto rs1_reg = inst->getRs1Reg();
                const std::vector<uint8_t> value = getRegByteVector_(rs1_reg);
                src_regs_.emplace_back(getRegId(rs1_reg), value);
            }

            if (inst->hasRs2())
            {
                const auto rs2_reg = inst->getRs2Reg();
                const std::vector<uint8_t> value = getRegByteVector_(rs2_reg);
                src_regs_.emplace_back(getRegId(rs2_reg), value);
            }

            // Get initial value of destination registers
            if (inst->hasRd())
            {
                const auto rd_reg = inst->getRdReg();
                const std::vector<uint8_t> value = getRegByteVector_(rd_reg);
                dst_regs_.emplace_back(getRegId(rd_reg), value);
            }

            // For instructions that read/write CSRs
            if (inst->hasCsr())
            {
                const auto csr_reg = state->getCsrRegister(inst->getCsr());
                if (csr_reg)
                {
                    const std::vector<uint8_t> value = getRegByteVector_(csr_reg);
                    dst_regs_.emplace_back(getRegId(csr_reg), value);
                }
            }
        }

        return nullptr;
    }

    template <typename XLEN> ActionGroup* InstructionLogger<XLEN>::preException_(AtlasState* state)
    {
        preExecute(state);

        // Get value of source registers
        fault_cause_ = state->getExceptionUnit()->getUnhandledFault();
        interrupt_cause_ = state->getExceptionUnit()->getUnhandledInterrupt();
        return nullptr;
    }

    template <typename XLEN> ActionGroup* InstructionLogger<XLEN>::postExecute_(AtlasState* state)
    {
        // Get final value of destination registers
        AtlasInstPtr inst = state->getCurrentInst();

        if (fault_cause_.isValid() == false)
        {
            sparta_assert(inst != nullptr, "Instruction is not valid for logging!");
        }

        if (inst)
        {
            for (auto & dst_reg : dst_regs_)
            {
                sparta::Register* reg = nullptr;
                switch (dst_reg.reg_id.reg_type)
                {
                    case RegType::INTEGER:
                        reg = state->getIntRegister(dst_reg.reg_id.reg_num);
                        break;
                    case RegType::FLOATING_POINT:
                        reg = state->getFpRegister(dst_reg.reg_id.reg_num);
                        break;
                    case RegType::VECTOR:
                        reg = state->getVecRegister(dst_reg.reg_id.reg_num);
                        break;
                    case RegType::CSR:
                        reg = state->getCsrRegister(dst_reg.reg_id.reg_num);
                        break;
                    default:
                        sparta_assert(false, "Invalid register type!");
                }
                sparta_assert(reg != nullptr);
                const std::vector<uint8_t> value = getRegByteVector_(reg);
                dst_reg.setValue(value);
            }
        }

        const uint32_t width = std::is_same_v<XLEN, RV64> ? 16 : 8;

        // Write to instruction logger
        const auto & symbols = state->getAtlasSystem()->getSymbols();
        if (symbols.find(pc_) != symbols.end())
        {
            INSTLOG("Call <" << symbols.at(pc_) << ">");
        }

        if (inst)
        {
            INSTLOG(HEX(pc_, width) << ": " << inst->dasmString() << " (" << HEX8(opcode_)
                                    << ") uid:" << inst->getUid());
        }
        else
        {
            // TODO: Only display opcode for certain exception types
            INSTLOG(HEX(pc_, width) << ": ??? (" << HEX8(opcode_) << ") uid: ?");
        }

        if (fault_cause_.isValid())
        {
            INSTLOG("trap cause: " << fault_cause_.getValue() << " ("
                    << HEX8(static_cast<uint32_t>(fault_cause_.getValue())) << ')');
        }

        if (inst && inst->hasImmediate())
        {
            using SXLEN = typename std::make_signed<XLEN>::type;
            const SXLEN imm_val = inst->getImmediate();
            INSTLOG("       imm: " << HEX(imm_val, width));
        }

        // TODO: Support for different register sizes (RV32, vector)
        for (const auto & src_reg : src_regs_)
        {
            const uint32_t reg_width = src_reg.reg_value.size() * 2;
            const uint64_t reg_value = getRegValue_(src_reg.reg_value);
            INSTLOG("   src " << std::setfill(' ') << std::setw(3) << src_reg.reg_id.reg_name
                              << ": " << HEX(reg_value, reg_width));
        }

        for (const auto & dst_reg : dst_regs_)
        {
            const uint32_t reg_width = dst_reg.reg_value.size() * 2;
            const uint64_t reg_value = getRegValue_(dst_reg.reg_value);
            const uint64_t reg_prev_value = getRegValue_(dst_reg.reg_prev_value);
            INSTLOG("   dst " << std::setfill(' ') << std::setw(3) << dst_reg.reg_id.reg_name
                              << ": " << HEX(reg_value, reg_width)
                              << " (prev: " << HEX(reg_prev_value, reg_width) << ")");
        }

        for (const auto & mem_read : mem_reads_)
        {
            INSTLOG("   mem read:  addr: " << HEX(mem_read.addr, width)
                                           << ", size: " << mem_read.size
                                           << ", value: " << HEX(mem_read.value, width));
        }

        for (const auto & mem_write : mem_writes_)
        {
            INSTLOG("   mem write: addr: "
                    << HEX(mem_write.addr, width) << ", size: " << mem_write.size
                    << ", value: " << HEX(mem_write.value, width)
                    << " (prev: " << HEX(mem_write.prior_value, width) << ")");
        }

        INSTLOG("");
        return nullptr;
    }
} // namespace atlas
