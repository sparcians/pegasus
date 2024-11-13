#include "core/inst_handlers/rv64/f/RvfInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RvfInsts::fmax_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // bool greater = f32_lt_quiet(FRS2_F, FRS1_F) ||
        //                              (f32_eq(FRS2_F, FRS1_F) && (FRS2_F.v & F32_SIGN));
        // if (isNaNF32UI(FRS1_F.v) && isNaNF32UI(FRS2_F.v))
        //     WRITE_FRD_F(f32(defaultNaNF32UI));
        // else
        //     WRITE_FRD_F((greater || isNaNF32UI(FRS2_F.v) ? FRS1_F : FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas