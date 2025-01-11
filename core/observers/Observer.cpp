#include "core/observers/Observer.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{

    void Observer::insertPostExecuteAction(ActionGroup* action_group)
    {
        if (post_execute_action_)
        {
            action_group->addAction(post_execute_action_);
        }
    }

    void Observer::insertPreExecuteAction(ActionGroup* action_group)
    {
        if (pre_execute_action_)
        {
            action_group->insertActionBefore(pre_execute_action_, ActionTags::EXECUTE_TAG);
        }
    }

} // namespace atlas
