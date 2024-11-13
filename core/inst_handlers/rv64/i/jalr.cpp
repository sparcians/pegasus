#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::jalr_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // CHECK_RD();
        uint64_t rd_val = state->getPc() + insn->getOpcodeSize();
        const uint64_t rs1_val = insn->getRs1()->read();
        constexpr uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t jump_target = (rs1_val + imm) & ~int64_t(1);
        state->setNextPc(jump_target);
        insn->getRd()->write(rd_val);

        // if (ZICFILP_xLPE(STATE.v, STATE.prv)) {
        //     STATE.elp = ZICFILP_IS_LP_EXPECTED(insn.rs1());
        //     serialize();
        // }

        return nullptr;
    }
} // namespace atlas
