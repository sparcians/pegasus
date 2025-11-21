#include "core/inst_handlers/zihintpause/RvzihintpauseInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzihintpauseInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "pause", pegasus::Action::createAction<&RvzihintpauseInsts::pauseHandler_<XLEN>,
                                                   RvzihintpauseInsts>(nullptr, "pause",
                                                                       ActionTags::EXECUTE_TAG));
    }

    template void RvzihintpauseInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzihintpauseInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN>
    Action::ItrType RvzihintpauseInsts::pauseHandler_(pegasus::PegasusState* state,
                                                      Action::ItrType action_it)
    {
        state->pauseHart(SimPauseReason::PAUSE);
        return ++action_it;
    }

} // namespace pegasus
