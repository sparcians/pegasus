#pragma once

#include "core/ActionGroup.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

namespace pegasus
{
    class PegasusState;

    class Execute : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "Execute";
        using base_type = Execute;

        class ExecuteParameters : public sparta::ParameterSet
        {
          public:
            ExecuteParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}
        };

        Execute(sparta::TreeNode* execute_node, const ExecuteParameters* p);

        ActionGroup* getActionGroup() { return &execute_action_group_; }

      private:
        Action::ItrType execute_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup execute_action_group_{"Execute"};
    };
} // namespace pegasus
