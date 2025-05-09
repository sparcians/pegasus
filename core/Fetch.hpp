#pragma once

#include "core/ActionGroup.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

namespace atlas
{
    class AtlasState;

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
        };

        Fetch(sparta::TreeNode* fetch_node, const FetchParameters* p);

        ActionGroup* getActionGroup() { return &fetch_action_group_; }

      private:
        AtlasState* state_ = nullptr;

        void onBindTreeEarly_() override;

        Action* fetch_(atlas::AtlasState* state, Action* action);

        ActionGroup fetch_action_group_{"Fetch"};

        Action* decode_(atlas::AtlasState* state, Action* action);

        ActionGroup decode_action_group_{"Decode"};

        void advanceSim_();
    };
} // namespace atlas
