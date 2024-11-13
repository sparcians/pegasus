#include "core/inst_handlers/rv64/d/RvdInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RvdInsts::fmax_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // bool greater = f64_lt_quiet(FRS2_D, FRS1_D) ||
        //                             (f64_eq(FRS2_D, FRS1_D) && (FRS2_D.v & F64_SIGN));
        // if (isNaNF64UI(FRS1_D.v) && isNaNF64UI(FRS2_D.v))
        //     WRITE_FRD_D(f64(defaultNaNF64UI));
        // else
        //     WRITE_FRD_D((greater || isNaNF64UI(FRS2_D.v) ? FRS1_D : FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas