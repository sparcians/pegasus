#include "core/Exception.hpp"
#include "core/Fetch.hpp"
#include "core/AtlasState.hpp"
#include "include/ActionTags.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace atlas
{

Exception::Exception(sparta::TreeNode* exception_node, const ExceptionParameters*)
    : sparta::Unit(exception_node)
{
    Action exception_action = atlas::Action::createAction<&Exception::handleException_>(this, "Exception");
    exception_action.addTag(ActionTags::EXCEPTION_TAG);
    exception_action_group_.addAction(exception_action);
}

void Exception::onBindTreeEarly_()
{
    auto core_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
    state_ = core_tn->getResourceAs<AtlasState>();
}

ActionGroup* Exception::handleException_(atlas::AtlasState* state)
{
    (void)state;
    return nullptr;
}

} // namespace atlas
