#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::jal_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // CHECK_RD();
        int64_t rd_val = state->getPc() + insn->getOpcodeSize();
        const uint64_t jump_target = state->getPc() + insn->getImmediate();
        state->setNextPc(jump_target);
        insn->getRd()->write(rd_val);

        return nullptr;
    }
} // namespace atlas