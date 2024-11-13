#include "core/inst_handlers/rv64/d/RvdInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RvdInsts::fmv_d_x_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('D');
        // require_rv64;
        // require_fp;
        // WRITE_FRD(f64(RS1));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas