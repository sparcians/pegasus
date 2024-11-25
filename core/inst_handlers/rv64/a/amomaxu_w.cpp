#include "core/inst_handlers/rv64/a/RvaInsts.hpp"

// #include "core/ActionGroup.hpp"
// #include "core/AtlasState.hpp"
// #include "core/AtlasInst.hpp"

namespace atlas
{
    class AtlasState;

    ActionGroup* RvaInsts::amomaxu_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amomaxu_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](uint32_t lhs) { return std::max(lhs,
        // uint32_t(RS2)); })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas