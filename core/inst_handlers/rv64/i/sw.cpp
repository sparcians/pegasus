#include "core/inst_handlers/rv64/i/RviInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RviInsts::sw_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint32_t));
        return nullptr;
    }

    ActionGroup* RviInsts::sw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs2_val = insn->getRs2()->read();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        state->writeMemory<uint32_t>(paddr, rs2_val);
        return nullptr;
    }
} // namespace atlas