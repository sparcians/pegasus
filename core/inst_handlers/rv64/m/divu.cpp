#include "core/inst_handlers/rv64/m/RvmInsts.hpp"

//#include "core/ActionGroup.hpp"
//#include "core/AtlasState.hpp"
//#include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvmInsts::divu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // reg_t lhs = zext_xlen(RS1);
        // reg_t rhs = zext_xlen(RS2);
        // if (rhs == 0)
        //     WRITE_RD(UINT64_MAX);
        // else
        //     WRITE_RD(sext_xlen(lhs / rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas