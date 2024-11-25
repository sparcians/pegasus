#include "core/inst_handlers/rv64/f/RvfInsts.hpp"

// #include "core/ActionGroup.hpp"
// #include "core/AtlasState.hpp"
// #include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvfInsts::fsqrt_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_sqrt(FRS1_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas