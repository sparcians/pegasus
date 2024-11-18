#include "core/inst_handlers/rv64/m/RvmInsts.hpp"

//#include "core/ActionGroup.hpp"
//#include "core/AtlasState.hpp"
//#include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvmInsts::mulhsu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('M', EXT_ZMMUL);
        // if (xlen == 64)
        //     WRITE_RD(mulhsu(RS1, RS2));
        // else
        //     WRITE_RD(sext32((sext32(RS1) * reg_t((uint32_t)RS2)) >> 32));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas