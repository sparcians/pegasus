#include "core/Execute.hpp"
#include "core/PegasusInst.hpp"
#include "core/PegasusCore.hpp"
#include "core/translate/Translate.hpp"
#include "include/ActionTags.hpp"

#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{
    Execute::Execute(sparta::TreeNode* execute_node, const ExecuteParameters*) :
        sparta::Unit(execute_node)
    {
        Action execute_action = pegasus::Action::createAction<&Execute::execute_>(this, "Execute");
        execute_action.addTag(ActionTags::EXECUTE_TAG);
        execute_action_group_.addAction(execute_action);
    }

    Action::ItrType Execute::execute_(PegasusState* state, Action::ItrType action_it)
    {
        const InstHandlers* inst_handlers = state->getCore()->getInstHandlers();
        // Connect instruction to Fetch
        const auto inst = state->getCurrentInst();
        ActionGroup* inst_action_group = inst->getActionGroup();
        inst_action_group->setNextActionGroup(state->getFinishActionGroup());

        // Insert translation Action into instruction's ActionGroup between the compute address
        // handler and the execute handler
        if (inst->isMemoryInst())
        {
            const ActionGroup* translate_action_group =
                inst->isStoreType() ? state->getTranslateUnit()->getStoreTranslateActionGroup()
                                    : state->getTranslateUnit()->getLoadTranslateActionGroup();
            for (auto it = translate_action_group->getActions().rbegin();
                 it != translate_action_group->getActions().rend(); ++it)
            {
                auto & action = *it;
                inst_action_group->insertActionAfter(action, ActionTags::COMPUTE_ADDR_TAG);
            }
        }

        if (inst->writesCsr())
        {
            const InstHandlers::CsrUpdateActionsMap* csr_update_actions =
                (state->getXlen() == 64) ? inst_handlers->getCsrUpdateActionsMap<RV64>()
                                         : inst_handlers->getCsrUpdateActionsMap<RV32>();
            auto action_it = csr_update_actions->find(inst->getCsr());
            if (action_it != csr_update_actions->end())
            {
                auto & action = action_it->second;
                inst_action_group->insertActionAfter(action, ActionTags::EXECUTE_TAG);
            }
        }

        state->insertExecuteActions(inst_action_group, inst->isMemoryInst());

        ILOG(inst);

        // Execute the instruction
        execute_action_group_.setNextActionGroup(inst_action_group);
        return ++action_it;
    }
} // namespace pegasus
