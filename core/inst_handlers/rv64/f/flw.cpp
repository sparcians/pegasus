#include "core/inst_handlers/rv64/f/RvfInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RvfInsts::flw_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvfInsts::flw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('F');
        // require_fp;
        // WRITE_FRD(f32(MMU.load<uint32_t>(RS1 + insn.i_imm())));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas