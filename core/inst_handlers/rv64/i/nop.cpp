#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/inst_handlers/rv64/i/RviInsts.hpp"

namespace atlas
{
    ActionGroup* RviInsts::nop_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }
} // namespace atlas