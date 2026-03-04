#include "core/inst_handlers/zvkned/RvzvknedInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzvknedInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        // inst_handlers.emplace("vaesdf_vs", pegasus::Action::createAction<&RvzvknedInsts::,
        // RvzvknedInsts>("vaesdf_vs", ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaesdf_vv",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaesdf_vv",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaesdm_vs",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaesdm_vs",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaesdm_vv",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaesdm_vv",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaesef_vs",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaesef_vs",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaesef_vv",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaesef_vv",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaesem_vs",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaesem_vs",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaesem_vv",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaesem_vv",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaeskf1_vi",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaeskf1_vi",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaeskf2_vi",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaeskf2_vi",
        // ActionTags::EXECUTE_TAG)); inst_handlers.emplace("vaesz_vs",
        // pegasus::Action::createAction<&RvzvknedInsts::, RvzvknedInsts>("vaesz_vs",
        // ActionTags::EXECUTE_TAG));
    }

    template void RvzvknedInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzvknedInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
