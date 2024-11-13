#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::lui_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t imm = insn->getSignExtendedImmediate<RV64, 32>();
        insn->getRd()->write(imm);

        return nullptr;
    }
} // namespace atlas
