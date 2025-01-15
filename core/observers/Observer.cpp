#include "core/observers/Observer.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{

    void Observer::insertPreExecuteActions(ActionGroup* action_group)
    {
        if (pre_execute_action_)
        {
            action_group->insertActionBefore(pre_execute_action_, ActionTags::EXECUTE_TAG);
        }
    }

    void Observer::insertPreExceptionActions(ActionGroup* action_group)
    {
        if (pre_exception_action_)
        {
            action_group->insertActionBefore(pre_exception_action_, ActionTags::EXCEPTION_TAG);
        }
    }

    void Observer::insertFinishActions(ActionGroup* action_group)
    {
        if (post_execute_action_)
        {
            action_group->addAction(post_execute_action_);
        }
    }

} // namespace atlas
