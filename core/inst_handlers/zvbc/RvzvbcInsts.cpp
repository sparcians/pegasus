#include "core/inst_handlers/zvbc/RvzvbcInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzvbcInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        //inst_handlers.emplace("vclmul_vv", pegasus::Action::createAction<&RvzvbcInsts::, RvzvbcInsts>("vclmul_vv", ActionTags::EXECUTE_TAG));
        //inst_handlers.emplace("vclmul_vx", pegasus::Action::createAction<&RvzvbcInsts::, RvzvbcInsts>("vclmul_vx", ActionTags::EXECUTE_TAG));
        //inst_handlers.emplace("vclmulh_vv", pegasus::Action::createAction<&RvzvbcInsts::, RvzvbcInsts>("vclmulh_vv", ActionTags::EXECUTE_TAG));
        //inst_handlers.emplace("vclmulh_vx", pegasus::Action::createAction<&RvzvbcInsts::, RvzvbcInsts>("vclmulh_vx", ActionTags::EXECUTE_TAG));
    }

    template void RvzvbcInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzvbcInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
