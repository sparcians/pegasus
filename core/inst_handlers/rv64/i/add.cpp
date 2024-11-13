#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::add_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();
        const uint64_t rd_val = rs1_val + rs2_val;
        insn->getRd()->write(rd_val);

        return nullptr;
    }
} // namespace atlas
