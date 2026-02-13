#include "core/inst_handlers/zawrs/RvzawrsInsts.hpp"
#include "core/PegasusState.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzawrsInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "wrs_nto",
            pegasus::Action::createAction<&RvzawrsInsts::wrsntoHandler_<XLEN>, RvzawrsInsts>(
                nullptr, "wrs_nto", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "wrs_sto",
            pegasus::Action::createAction<&RvzawrsInsts::wrsstoHandler_<XLEN>, RvzawrsInsts>(
                nullptr, "wrs_sto", ActionTags::EXECUTE_TAG));
    }

    template void RvzawrsInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzawrsInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN>
    Action::ItrType RvzawrsInsts::wrsntoHandler_(PegasusState* state, Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        state->pauseHart(SimPauseReason::WRS_NTO);
        state->registerWaitOnReservationSetCB();

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzawrsInsts::wrsstoHandler_(PegasusState* state, Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        state->pauseHart(SimPauseReason::WRS_STO);
        state->registerWaitOnReservationSetCB();

        return ++action_it;
    }
} // namespace pegasus
