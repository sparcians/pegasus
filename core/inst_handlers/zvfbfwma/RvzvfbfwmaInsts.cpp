#include "core/inst_handlers/zvfbfwma/RvzvfbfwmaInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzvfbfwmaInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        // inst_handlers.emplace("vfwmaccbf16_vv", pegasus::Action::createAction<&RvzvfbfwmaInsts::,
        // RvzvfbfwmaInsts>("vfwmaccbf16_vv", ActionTags::EXECUTE_TAG));
        // inst_handlers.emplace("vfwmaccbf16_vf", pegasus::Action::createAction<&RvzvfbfwmaInsts::,
        // RvzvfbfwmaInsts>("vfwmaccbf16_vf", ActionTags::EXECUTE_TAG));
    }

    template void RvzvfbfwmaInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzvfbfwmaInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
