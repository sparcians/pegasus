#include "core/inst_handlers/zvksh/RvzvkshInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzvkshInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        // inst_handlers.emplace("vsm3c_vi", pegasus::Action::createAction<&RvzvkshInsts::,
        // RvzvkshInsts>("vsm3c_vi", ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vsm3me_vv",
        // pegasus::Action::createAction<&RvzvkshInsts::, RvzvkshInsts>("vsm3me_vv",
        // ActionTags::EXECUTE_TAG));
    }

    template void RvzvkshInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzvkshInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
