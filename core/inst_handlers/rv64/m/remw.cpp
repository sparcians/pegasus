#include "core/inst_handlers/rv64/m/RvmInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RvmInsts::remw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // require_rv64;
        // sreg_t lhs = sext32(RS1);
        // sreg_t rhs = sext32(RS2);
        // if (rhs == 0)
        //     WRITE_RD(lhs);
        // else
        //     WRITE_RD(sext32(lhs % rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas