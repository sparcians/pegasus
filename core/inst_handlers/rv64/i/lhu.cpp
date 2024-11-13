#include "core/inst_handlers/rv64/i/RviInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RviInsts::lhu_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint16_t));
        return nullptr;
    }

    ActionGroup* RviInsts::lhu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = state->readMemory<uint16_t>(paddr);
        insn->getRd()->write(rd_val);
        return nullptr;
    }
} // namespace atlas