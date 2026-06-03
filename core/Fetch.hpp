#pragma once

#include "core/ActionGroup.hpp"
#include "core/translate/ExecutionPage.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

#include <map>
#include <memory>
#include <tuple>

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
            PARAMETER(bool, enable_execution_cache, false,
                      "Enable the experimental translated-page execution cache fast path")
        };

        Fetch(sparta::TreeNode* fetch_node, const FetchParameters* p);

        ActionGroup* getActionGroup() { return &fetch_action_group_; }

      private:
        PegasusState* state_ = nullptr;
        const bool enable_execution_cache_ = false;
        ActionGroup* execute_action_group_ = nullptr;

        // ExecutionPageKey is a tuple of (virt_page_base_addr, phys_page_base_addr, page_offset)
        // It allows us to identify an execution page
        using ExecutionPageKey = std::tuple<Addr, Addr, Addr>;
        // The actual map of execution pages, keyed by the above tuple
        std::map<ExecutionPageKey, std::unique_ptr<ExecutionPage>> execution_pages_;

        void onBindTreeEarly_() override;

        Action::ItrType fetch_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup fetch_action_group_{"Fetch"};

        // Action group for dispatching a fetch that has already been translated to an ExecutionPage
        Action::ItrType dispatchTranslatedFetch_(pegasus::PegasusState* state,
                                                Action::ItrType action_it);

        ActionGroup translated_fetch_dispatch_action_group_{"Dispatch Translated Fetch"};

        Action::ItrType decode_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup decode_action_group_{"Decode"};
    };
} // namespace pegasus
