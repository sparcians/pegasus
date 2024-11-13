#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::mv_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->read();
        insn->getRd()->write(rs1_val);

        return nullptr;
    }
} // namespace atlas