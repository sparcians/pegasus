#include "core/inst_handlers/rv64/a/RvaInsts.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    ActionGroup* RvaInsts::amomin_w_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvaInsts::amomin_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('A');
        // WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](int32_t lhs) { return std::min(lhs,
        // int32_t(RS2)); })));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }
} // namespace atlas