#include "core/inst_handlers/zvkg/RvzvkgInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzvkgInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        //inst_handlers.emplace("vghsh_vv", pegasus::Action::createAction<&RvzvkgInsts::, RvzvkgInsts>("vghsh_vv", ActionTags::EXECUTE_TAG));
        //inst_handlers.emplace("vgmul_vv", pegasus::Action::createAction<&RvzvkgInsts::, RvzvkgInsts>("vgmul_vv", ActionTags::EXECUTE_TAG));
    }

    template void RvzvkgInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzvkgInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
