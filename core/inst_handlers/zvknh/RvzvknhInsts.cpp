#include "core/inst_handlers/zvknh/RvzvknhInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzvknhInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        //inst_handlers.emplace("vsha2ch_vv", pegasus::Action::createAction<&RvzvknhInsts::, RvzvknhInsts>("vsha2ch_vv", ActionTags::EXECUTE_TAG));
        //inst_handlers.emplace("vsha2cl_vv", pegasus::Action::createAction<&RvzvknhInsts::, RvzvknhInsts>("vsha2cl_vv", ActionTags::EXECUTE_TAG));
        //inst_handlers.emplace("vsha2ms_vv", pegasus::Action::createAction<&RvzvknhInsts::, RvzvknhInsts>("vsha2ms_vv", ActionTags::EXECUTE_TAG));
    }

    template void RvzvknhInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzvknhInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
