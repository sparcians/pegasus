#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::slti_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = insn->getRs1()->read();
        const uint32_t IMM_SIZE = 12;
        const int64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const int64_t rd_val = rs1_val < imm;
        insn->getRd()->write(rd_val);

        return nullptr;
    }
} // namespace atlas