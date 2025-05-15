#pragma once

#include "core/Action.hpp"

#include "sparta/utils/SpartaAssert.hpp"

#include <algorithm>

namespace atlas
{
    class ActionException : public std::exception
    {
      public:
        ActionException(ActionGroup* action_group) : action_group_(action_group) {}

        ActionGroup* getActionGroup() { return action_group_; }

        const char* what() const throw();

      private:
        ActionGroup* action_group_;
    };

    /*
     * \class ActionGroup
     *
     * \brief A group of Actions
     *
     * An ActionGroup is a group of related Actions that are expected to be
     * executed together. A group of Actions can (and should) be collected
     * into an ActionGroup if the following requirements are met:
     * 1. The group of Actions are always executed in order
     *
     * If an ActionGroup contains Actions A, B and C, Action A is always
     * executed first followed by Action B and Action C.
     *
     * 2. The group of Actions only has one entry point
     *
     * If an ActionGroup contains Actions A, B and C, execution will always
     * begin with Action A. It can NEVER begin with Action B or Action C.
     *
     * 3. The group of Actions may have more than one exit point
     *
     * If an ActionGroup contains Actions A, B and C, it is possible for its
     * execution to be stopped after either Action A, Action B or Action C.
     *
     * 4. The ActionGroup execution cannot be continued after it has been stopped
     * If an ActionGroup contains Actions A, B and C, and execution is stopped
     * after Action A or Action B, execution cannot be continued from Action B
     * or Action C
     *
     */

    class ActionGroup
    {
      public:
        ActionGroup(const std::string & name) : name_(name) {}

        ActionGroup(const std::string & name, const Action & action) :
            name_(name),
            action_group_{action}
        {
        }

        // Not default -- defined in source file to reduce massive inlining
        ~ActionGroup();

        const std::string & getName() const { return name_; }

        const std::vector<Action> & getActions() const { return action_group_; };

        ActionGroup* execute(AtlasState* state)
        {
            fail_action_group_ = nullptr;
            Action::ItrType action_it = action_group_.begin();
            while (action_it != action_group_.end())
            {
                try
                {
                    // Actions are responsible for incrementing the Action iterator. If an Action
                    // needs to be repeated, the Action iterator will be returned without being
                    // incremented.
                    action_it = action_it->execute(state, action_it);
                }
                catch (ActionException & action_excp)
                {
                    return action_excp.getActionGroup();
                }
            }

            return next_action_group_;
        }

        //! Add Action to the back of the group
        void addAction(const Action & action) { action_group_.emplace_back(action); }

        //! Add Action to the front of the group
        void insertActionFront(const Action & action)
        {
            action_group_.insert(action_group_.begin(), action);
        }

        //! Insert Action before the first Action in the group with the specified Tag
        void insertActionBefore(const Action & action, const ActionTagType tag)
        {
            auto action_it = findActionWithTag_(tag);
            action_group_.insert(action_it, action);
        }

        //! Insert Action after the first Action in the group with the specified Tag
        void insertActionAfter(const Action & action, const ActionTagType tag)
        {
            auto action_it = findActionWithTag_(tag);
            action_group_.insert(++action_it, action);
        }

        //! Remove all Actions from the group with the specified Tag
        void removeAction(const ActionTagType tag)
        {
            const auto num_erased = std::erase_if(action_group_, [tag](const Action & action)
                                                  { return action.hasTag(tag); });
            sparta_assert(num_erased != 0, "Failed to remove any Actions with tag "
                                               << ActionTagFactory::getTagName(tag).c_str()
                                               << " from the ActionGroup: " << this);
        }

        //! Replace an Action with the specified Tag
        void replaceAction(const ActionTagType tag, const Action & action)
        {
            auto action_it = findActionWithTag_(tag);
            *action_it = action;
        }

        void setNextActionGroup(ActionGroup* next_action_group)
        {
            next_action_group_ = next_action_group;
        }

        ActionGroup* getNextActionGroup() const
        {
            assert(next_action_group_);
            return next_action_group_;
        }

        bool hasTag(ActionTagType tag) const
        {
            if (SPARTA_EXPECT_TRUE(action_group_.empty() == false))
            {
                return action_group_.front().hasTag(tag);
            }
            return false;
        }

      private:
        const std::string name_;

        std::vector<Action> action_group_;

        ActionGroup* next_action_group_ = nullptr;

        //! Find first Action in the group with the specified Tag
        std::vector<Action>::iterator findActionWithTag_(const ActionTagType tag)
        {
            std::vector<Action>::iterator action_it =
                std::find_if(action_group_.begin(), action_group_.end(),
                             [tag](const Action & action) { return action.hasTag(tag); });

            sparta_assert(action_it != action_group_.end(),
                          "Failed to find any Actions with tag "
                              << ActionTagFactory::getTagName(tag).c_str()
                              << " in the ActionGroup: " << this);
            return action_it;
        }

        friend std::ostream & operator<<(std::ostream & os, const ActionGroup & action_group);
        friend std::ostream & operator<<(std::ostream & os, const ActionGroup* action_group);
    };

    inline std::ostream & operator<<(std::ostream & os, const ActionGroup & action_group)
    {
        const size_t size = action_group.action_group_.size();
        for (size_t idx = 0; idx < size; ++idx)
        {
            const Action & action = action_group.action_group_[idx];
            os << "[" << action << " (" << ActionTagFactory::getTagName(action.getTag())
               << ")] -> ";
        }
        os << (action_group.next_action_group_ ? action_group.next_action_group_->getName() : "?");
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const ActionGroup* action_group)
    {
        os << *action_group;
        return os;
    }
} // namespace atlas
