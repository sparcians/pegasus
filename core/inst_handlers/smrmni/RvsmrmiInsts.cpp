#include "core/inst_handlers/i/RvsmrmniInsts.hpp"
#include "core/inst_handlers/i/RviFunctors.hpp"
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
    void RvsmrmniInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers,
                                   bool enable_syscall_emulation)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        constexpr bool SIGN_EXTEND = true;

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "mnret",
                pegasus::Action::createAction<&RvsmrmniInsts::xretHandler_<RV64, PrivMode::MACHINE>,
                                              RvsmrmniInsts>(nullptr, "mnret", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "mnret",
                pegasus::Action::createAction<&RvsmrmniInsts::xretHandler_<RV32, PrivMode::MACHINE>,
                                              RvsmrmniInsts>(nullptr, "mnret", ActionTags::EXECUTE_TAG));
        }
    }

    template <typename XLEN, PrivMode PRIV_MODE>
    Action::ItrType RvsmrmniInsts::xretHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        sparta_assert(false, "mnret is not implemented");
        return ++action_it;
    }

} // namespace pegasus
