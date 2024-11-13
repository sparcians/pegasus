#include "core/inst_handlers/rv64/d/RvdInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RvdInsts::fcvt_d_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f32_to_f64(FRS1_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas