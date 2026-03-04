#include "core/inst_handlers/zvksed/RvzvksedInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzvksedInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        //inst_handlers.emplace("vsm4k_vi", pegasus::Action::createAction<&RvzvksedInsts::, RvzvksedInsts>("vsm4k_vi", ActionTags::EXECUTE_TAG));
        //inst_handlers.emplace("vsm4r_vv", pegasus::Action::createAction<&RvzvksedInsts::, RvzvksedInsts>("vsm4r_vv", ActionTags::EXECUTE_TAG));
        //inst_handlers.emplace("vsm4r_vs", pegasus::Action::createAction<&RvzvksedInsts::, RvzvksedInsts>("vsm4r_vs", ActionTags::EXECUTE_TAG));
    }

    template void RvzvksedInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzvksedInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
