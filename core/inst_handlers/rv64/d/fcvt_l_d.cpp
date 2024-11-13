#include "core/inst_handlers/rv64/d/RvdInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RvdInsts::fcvt_l_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_rv64;
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_RD(f64_to_i64(FRS1_D, RM, true));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas