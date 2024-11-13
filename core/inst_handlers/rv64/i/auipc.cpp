#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::auipc_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 32;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t pc = state->getPc();
        const uint64_t rd_val =
            ((int64_t)(imm + pc) << (64 - (state->getXlen()))) >> (64 - (state->getXlen()));
        insn->getRd()->write(rd_val);

        return nullptr;
    }
} // namespace atlas
