#pragma once

#include "core/ActionGroup.hpp"
#include "include/AtlasTypes.hpp"

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

        template <typename XLEN> const InstHandlersMap* getInstHandlersMap() const
        {
            static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return &rv64_inst_handlers_;
            }
            else
            {
                return &rv32_inst_handlers_;
            }
        }

        template <typename XLEN> const InstHandlersMap* getInstComputeAddressHandlersMap() const
        {
            static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return &rv64_inst_compute_address_handlers_;
            }
            else
            {
                return &rv32_inst_compute_address_handlers_;
            }
        }

      private:
        ActionGroup* execute_(atlas::AtlasState* state);

        ActionGroup execute_action_group_{"Execute"};

        // Instruction handlers
        InstHandlersMap rv64_inst_handlers_;
        InstHandlersMap rv32_inst_handlers_;

        // Instruction handlers for computing the address of load/store instructions
        InstHandlersMap rv64_inst_compute_address_handlers_;
        InstHandlersMap rv32_inst_compute_address_handlers_;
    };
} // namespace atlas
