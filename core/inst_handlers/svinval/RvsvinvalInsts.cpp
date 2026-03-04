#include "core/inst_handlers/svinval/RvsvinvalInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvsvinvalInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        // inst_handlers.emplace("sinval_vma", pegasus::Action::createAction<&RvsvinvalInsts::,
        // RvsvinvalInsts>("sinval_vma", ActionTags::EXECUTE_TAG));
        // inst_handlers.emplace("sfence_w_inval", pegasus::Action::createAction<&RvsvinvalInsts::,
        // RvsvinvalInsts>("sfence_w_inval", ActionTags::EXECUTE_TAG));
        // inst_handlers.emplace("hinval_vvma", pegasus::Action::createAction<&RvsvinvalInsts::,
        // RvsvinvalInsts>("hinval_vvma", ActionTags::EXECUTE_TAG));
        // inst_handlers.emplace("hinval_gvma", pegasus::Action::createAction<&RvsvinvalInsts::,
        // RvsvinvalInsts>("hinval_gvma", ActionTags::EXECUTE_TAG));
    }

    template void RvsvinvalInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvsvinvalInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
