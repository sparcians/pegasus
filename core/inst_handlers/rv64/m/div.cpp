#include "core/inst_handlers/rv64/m/RvmInsts.hpp"

//#include "core/ActionGroup.hpp"
//#include "core/AtlasState.hpp"
//#include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvmInsts::div_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('M');
        // sreg_t lhs = sext_xlen(RS1);
        // sreg_t rhs = sext_xlen(RS2);
        // if (rhs == 0)
        //     WRITE_RD(UINT64_MAX);
        // else if (lhs == INT64_MIN && rhs == -1)
        //     WRITE_RD(lhs);
        // else
        //     WRITE_RD(sext_xlen(lhs / rhs));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas