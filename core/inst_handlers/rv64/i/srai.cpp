#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::srai_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const int64_t rs1_val = insn->getRs1()->read();
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        const int64_t rd_val = (int64_t)(rs1_val >> shift_amount);
        insn->getRd()->write(rd_val);

        return nullptr;
    }
} // namespace atlas