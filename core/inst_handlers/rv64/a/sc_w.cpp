#include "core/inst_handlers/rv64/a/RvaInsts.hpp"

// #include "core/ActionGroup.hpp"
// #include "core/AtlasState.hpp"
// #include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvaInsts::sc_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::sc_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');

        // bool have_reservation = MMU.store_conditional<uint32_t>(RS1, RS2);

        // WRITE_RD(!have_reservation);

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas