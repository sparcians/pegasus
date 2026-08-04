#pragma once

#include "core/ActionGroup.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

namespace pegasus
{
    class PegasusState;

    class Fetch : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "Fetch";
        using base_type = Fetch;

        class FetchParameters : public sparta::ParameterSet
        {
          public:
            FetchParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            // Runtime parameter that allows for the execution cache to be used
            PARAMETER(bool, enable_ecache, false,
                      "Enable the experimental translated-page execution cache fast path")
        };

        Fetch(sparta::TreeNode* fetch_node, const FetchParameters* p);

        ActionGroup* getActionGroup() { return &fetch_action_group_; }

        // Returns the post-execute loopback action group.
        // Returns Fetch when ecache is disabled, otherwise returns Translate so the
        // post-execute path can hand off directly to translation.
        ActionGroup* getLoopbackActionGroup()
        {
            return enable_ecache_ ? translate_action_group_ : &fetch_action_group_;
        }

        // Conservative full flush of translated execution pages.
        void flushExecutionCache();

        bool isEcacheEnabled() const { return enable_ecache_; }

      private:
        PegasusState* state_ = nullptr;
        const FetchParameters* params_ = nullptr;
        bool enable_ecache_ = false;
        ActionGroup* execute_action_group_ = nullptr;
        ActionGroup* translate_action_group_ = nullptr;

        void onBindTreeEarly_() override;

        Action::ItrType fetch_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup fetch_action_group_{"Fetch"};

        Action::ItrType decode_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup decode_action_group_{"Decode"};
    };
} // namespace pegasus
