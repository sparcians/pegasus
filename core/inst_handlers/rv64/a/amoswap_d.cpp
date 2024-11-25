#include "core/inst_handlers/rv64/a/RvaInsts.hpp"

// #include "core/ActionGroup.hpp"
// #include "core/AtlasState.hpp"
// #include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvaInsts::amoswap_d_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amoswap_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // require_rv64;
        // WRITE_RD(MMU.amo<uint64_t>(RS1, [&](uint64_t UNUSED lhs) { return RS2; }));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas