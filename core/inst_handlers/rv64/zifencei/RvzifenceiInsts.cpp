#include "core/inst_handlers/rv64/zifencei/RvzifenceiInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    void RvzifenceiInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace(
            "fence.i",
            atlas::Action::createAction<&RvzifenceiInsts::fence_i_64_handler, RvzifenceiInsts>(
                nullptr, "fence_i", ActionTags::EXECUTE_TAG));
    }

    ActionGroup* RvzifenceiInsts::fence_i_64_handler(atlas::AtlasState* state)
    {
        state->getCurrentInst()->markUnimplemented();
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // MMU.flush_icache();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

} // namespace atlas
