#include "core/inst_handlers/zicboz/RvzicbozInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzicbozInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        // inst_handlers.emplace("cbo_zero", pegasus::Action::createAction<&RvzicbozInsts::,
        // RvzicbozInsts>("cbo_zero", ActionTags::EXECUTE_TAG));
    }

    template void RvzicbozInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzicbozInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
