#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::li_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        insn->getRd()->write(imm);

        return nullptr;
    }
} // namespace atlas
