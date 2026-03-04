#include "core/inst_handlers/zicfiss/RvzicfissInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzicfissInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        // inst_handlers.emplace("sspush", pegasus::Action::createAction<&RvzicfissInsts::,
        // RvzicfissInsts>("sspush", ActionTags::EXECUTE_TAG)); inst_handlers.emplace("sspopchk",
        // pegasus::Action::createAction<&RvzicfissInsts::, RvzicfissInsts>("sspopchk",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("ssrdp",
        // pegasus::Action::createAction<&RvzicfissInsts::, RvzicfissInsts>("ssrdp",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("ssamoswap_w",
        // pegasus::Action::createAction<&RvzicfissInsts::, RvzicfissInsts>("ssamoswap_w",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("ssamoswap_d",
        // pegasus::Action::createAction<&RvzicfissInsts::, RvzicfissInsts>("ssamoswap_d",
        // ActionTags::EXECUTE_TAG));
    }

    template void RvzicfissInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
