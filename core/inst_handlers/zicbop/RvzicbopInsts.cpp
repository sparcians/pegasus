#include "core/inst_handlers/zicbop/RvzicbopInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzicbopInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        // inst_handlers.emplace("prefetch_i", pegasus::Action::createAction<&RvzicbopInsts::,
        // RvzicbopInsts>("prefetch_i", ActionTags::EXECUTE_TAG));
        // inst_handlers.emplace("prefetch_r", pegasus::Action::createAction<&RvzicbopInsts::,
        // RvzicbopInsts>("prefetch_r", ActionTags::EXECUTE_TAG));
        // inst_handlers.emplace("prefetch_w", pegasus::Action::createAction<&RvzicbopInsts::,
        // RvzicbopInsts>("prefetch_w", ActionTags::EXECUTE_TAG));
    }

    template void RvzicbopInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzicbopInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
