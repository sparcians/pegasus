#include "core/inst_handlers/rv64/f/RvfInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RvfInsts::fsgnjx_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // WRITE_FRD_F(fsgnj32(freg(FRS1_F), freg(FRS2_F), false, true));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas