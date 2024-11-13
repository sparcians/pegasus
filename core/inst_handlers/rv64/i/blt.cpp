#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::blt_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = insn->getRs1()->read();
        const int64_t rs2_val = insn->getRs2()->read();

        if (rs1_val < rs2_val)
        {
            const uint64_t pc = state->getPc();
            const uint32_t IMM_SIZE = 13;
            const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
            const uint64_t branch_target = pc + imm;
            state->setNextPc(branch_target);
        }

        return nullptr;
    }
} // namespace atlas
