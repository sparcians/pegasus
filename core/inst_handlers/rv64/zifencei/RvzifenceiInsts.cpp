#include "core/inst_handlers/rv64/zifencei/RvzifenceiInsts.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    void RvzifenceiInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace(
            "fence.i",
            atlas::Action::createAction<&RvzifenceiInsts::fence_i_64_handler, RvzifenceiInsts>(
                nullptr, "fence_i", ActionTags::EXECUTE_TAG));
    }
}; // namespace atlas
