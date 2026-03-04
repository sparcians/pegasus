#include "core/inst_handlers/zfbfmin/RvzfbfminInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzfbfminInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        //inst_handlers.emplace("fcvt_bf16_s", pegasus::Action::createAction<&RvzfbfminInsts::, RvzfbfminInsts>("fcvt_bf16_s", ActionTags::EXECUTE_TAG));
        //inst_handlers.emplace("fcvt_s_bf16", pegasus::Action::createAction<&RvzfbfminInsts::, RvzfbfminInsts>("fcvt_s_bf16", ActionTags::EXECUTE_TAG));
    }

    template void RvzfbfminInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzfbfminInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
