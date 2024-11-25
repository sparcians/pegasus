#include "core/inst_handlers/rv64/m/RvmInsts.hpp"

// #include "core/ActionGroup.hpp"
// #include "core/AtlasState.hpp"
// #include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvmInsts::divuw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // require_rv64;
        // reg_t lhs = zext32(RS1);
        // reg_t rhs = zext32(RS2);
        // if (rhs == 0)
        //     WRITE_RD(UINT64_MAX);
        // else
        //     WRITE_RD(sext32(lhs / rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas