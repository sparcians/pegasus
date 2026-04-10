#include "core/inst_handlers/smrmni/RvsmrmniInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/PegasusUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusCore.hpp"
#include "core/PegasusInst.hpp"
#include "system/PegasusSystem.hpp"
#include "system/SystemCallEmulator.hpp"
#include "sparta/memory/SimpleMemoryMapNode.hpp"

#include <functional>

namespace pegasus
{
    template <typename XLEN>
    void RvsmrmniInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "mnret",
            pegasus::Action::createAction<&RvsmrmniInsts::xretHandler_<XLEN, PrivMode::MACHINE>,
                                          RvsmrmniInsts>(nullptr, "mnret",
                                                         ActionTags::EXECUTE_TAG));
    }

    template <typename XLEN, PrivMode PRIV_MODE>
    Action::ItrType RvsmrmniInsts::xretHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        (void)state;
        sparta_assert(false, "mnret is not implemented");
        return ++action_it;
    }

    template void RvsmrmniInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvsmrmniInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

} // namespace pegasus
