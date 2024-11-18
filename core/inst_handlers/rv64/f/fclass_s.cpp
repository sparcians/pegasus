#include "core/inst_handlers/rv64/f/RvfInsts.hpp"

//#include "core/ActionGroup.hpp"
//#include "core/AtlasState.hpp"
//#include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvfInsts::fclass_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // WRITE_RD(f32_classify(FRS1_F));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas