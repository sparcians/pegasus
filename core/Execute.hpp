#pragma once

#include "core/ActionGroup.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

namespace atlas
{
    class AtlasState;

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

        using InstHandlersMap = std::map<std::string, Action>;

        const InstHandlersMap* getInstHandlersMap() const { return &inst_handlers_; }

        const InstHandlersMap* getInstComputeAddressHandlersMap() const
        {
            return &inst_compute_address_handlers_;
        }

      private:
        ActionGroup* execute_(atlas::AtlasState* state);

        ActionGroup execute_action_group_{"Execute"};

        // Instruction handlers
        InstHandlersMap inst_handlers_;

        // Instruction handlers for computing the address of load/store instructions
        InstHandlersMap inst_compute_address_handlers_;
    };
} // namespace atlas
