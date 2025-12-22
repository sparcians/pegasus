#include "core/inst_handlers/h/RvhInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvhInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "hfence.vvma",
            pegasus::Action::createAction<&RvhInsts::hfenceHandler_<XLEN, false>, RvhInsts>(
                nullptr, "hfence.vvma", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "hfence.gvma",
            pegasus::Action::createAction<&RvhInsts::hfenceHandler_<XLEN, true>, RvhInsts>(
                nullptr, "hfence.gvma", ActionTags::EXECUTE_TAG));
    }

    template void RvhInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvhInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN, bool GUEST>
    Action::ItrType RvhInsts::hfenceHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PrivMode priv_mode = state->getPrivMode();
        if constexpr (GUEST)
        {
            // HFENCE.GVMA is valid only in HS-mode when mstatus.TVM=0,
            // or in M-mode (irrespective of mstatus.TVM)
            XLEN tvm = READ_CSR_FIELD<XLEN>(state, MSTATUS, "tvm");
            if (((tvm == 0) && (priv_mode != PrivMode::HYPERVISOR))
                || (priv_mode != PrivMode::MACHINE))
            {
                THROW_ILLEGAL_INST;
            }
        }
        else
        {
            // hfence.vvma is only valid in M-mode or HS-mode
            if (priv_mode < PrivMode::HYPERVISOR)
            {
                THROW_ILLEGAL_INST;
            }
        }

        // TODO: Flush any TLBs and instruction/block caches in the future
        return ++action_it;
    }

} // namespace pegasus
