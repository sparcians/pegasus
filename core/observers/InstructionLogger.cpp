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

    InstructionLogger::InstructionLogger(sparta::TreeNode* node) :
        inst_logger_(node, "inst", "Atlas Instruction Logger")
    {
        pre_execute_action_ =
            atlas::Action::createAction<&InstructionLogger::preExecute_>(this, "pre execute");
        post_execute_action_ =
            atlas::Action::createAction<&InstructionLogger::postExecute_>(this, "post execute");
    }

    ActionGroup* InstructionLogger::preExecute_(AtlasState* state)
    {
        reset_();

        pc_ = state->getPc();
        AtlasInstPtr inst = state->getCurrentInst();
        opcode_ = inst->getOpcode();

        // Get value of source registers
        if (inst->hasRs1())
        {
            const auto rs1 = inst->getRs1();
            const std::vector<uint8_t> value = convertToByteVector(rs1->dmiRead<uint64_t>());
            src_regs_.emplace_back(getRegId(rs1), value);
        }

        if (inst->hasRs2())
        {
            const auto rs2 = inst->getRs2();
            const std::vector<uint8_t> value = convertToByteVector(rs2->dmiRead<uint64_t>());
            src_regs_.emplace_back(getRegId(rs2), value);
        }

        // Get initial value of destination registers
        if (inst->hasRd())
        {
            const auto rd = inst->getRd();
            const std::vector<uint8_t> value = convertToByteVector(rd->dmiRead<uint64_t>());
            dst_regs_.emplace_back(getRegId(rd), value);
        }

        return nullptr;
    }

    ActionGroup* InstructionLogger::postExecute_(AtlasState* state)
    {
        sparta_assert(inst_logger_.observed(), "Instruction logging is not enabled");

        // Get final value of destination registers
        AtlasInstPtr inst = state->getCurrentInst();
        sparta_assert(inst != nullptr, "Instruction is not valid for logging!");

        if (!inst->hasRd()) {
            INSTLOG("core   0: " << (uint64_t)state->getPrivMode() /*<< " " << HEX16(state->getPc())*/
                                 << " " << "(" << HEX8(inst->getOpcode()) << ")");
        } else {
            INSTLOG("core   0: " << (uint64_t)state->getPrivMode() /*<< " " << HEX16(state->getPc())*/
                                 << " " << "(" << HEX8(inst->getOpcode()) << ")"
                                 << " " << "[" << inst->getRd()->getName()
                                 << " " << HEX16(inst->getRd()->dmiRead<uint64_t>()) << "]");
        }

        return nullptr;

        if (inst->hasRd())
        {
            sparta_assert(dst_regs_.size() == 1);
            const auto & rd = inst->getRd();
            const std::vector<uint8_t> value = convertToByteVector(rd->dmiRead<uint64_t>());
            dst_regs_[0].setValue(value);
        }

        // Write to instruction logger
        const auto & symbols = state->getAtlasSystem()->getSymbols();
        if (symbols.find(pc_) != symbols.end())
        {
            INSTLOG(HEX8(pc_) << " <" << symbols.at(pc_) << ">:");
        }

        INSTLOG(HEX8(pc_) << ": " << HEX8(opcode_) << " " << inst->dasmString());

        if (inst->hasImmediate())
        {
            const int64_t imm_val = inst->getImmediate();
            INSTLOG("       imm: " << HEX16(imm_val));
        }

        // TODO: Support for different register sizes (RV32, vector)
        for (const auto & src_reg : src_regs_)
        {
            const uint64_t reg_value = convertFromByteVector<uint64_t>(src_reg.reg_value);
            INSTLOG("   src " << std::setfill(' ') << std::setw(3) << src_reg.reg_id.reg_name
                              << ": " << HEX16(reg_value));
        }

        for (const auto & dst_reg : dst_regs_)
        {
            const uint64_t reg_value = convertFromByteVector<uint64_t>(dst_reg.reg_value);
            const uint64_t reg_prev_value = convertFromByteVector<uint64_t>(dst_reg.reg_prev_value);
            INSTLOG("   dst " << std::setfill(' ') << std::setw(3) << dst_reg.reg_id.reg_name
                              << ": " << HEX16(reg_value) << " (prev: " << HEX16(reg_prev_value)
                              << ")");
        }

        INSTLOG("");

        return nullptr;
    }
} // namespace atlas
