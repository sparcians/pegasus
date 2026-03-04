#include "core/inst_handlers/zicbom/RvzicbomInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzicbomInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        // inst_handlers.emplace("cbo_clean", pegasus::Action::createAction<&RvzicbomInsts::,
        // RvzicbomInsts>("cbo_clean", ActionTags::EXECUTE_TAG)); inst_handlers.emplace("cbo_flush",
        // pegasus::Action::createAction<&RvzicbomInsts::, RvzicbomInsts>("cbo_flush",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("cbo_inval",
        // pegasus::Action::createAction<&RvzicbomInsts::, RvzicbomInsts>("cbo_inval",
        // ActionTags::EXECUTE_TAG));
    }

    template void RvzicbomInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzicbomInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
