#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::sllw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t rs1_val = insn->getRs1()->read();
        const uint32_t rs2_val = insn->getRs2()->read();
        // Casting from int32_t to int64_t will sign extend the value
        const int64_t rd_val = (int64_t)(int32_t)(rs1_val << (rs2_val & 0x1F));
        insn->getRd()->write(rd_val);

        return nullptr;
    }
} // namespace atlas