#include "core/inst_handlers/rv64/d/RvdInsts.hpp"

//#include "core/ActionGroup.hpp"
//#include "core/AtlasState.hpp"
//#include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvdInsts::fnmsub_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_mulAdd(f64(FRS1_D.v ^ F64_SIGN), FRS2_D, FRS3_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas