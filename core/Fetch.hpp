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

        // Returns the post-execute loopback action group.
        // When ecache is enabled, this bypasses fetch_+translate for same-page reuse.
        // When ecache is disabled, this is the normal fetch_ action group.
        ActionGroup* getLoopbackActionGroup()
        {
            return enable_execution_cache_ ? &exec_page_loop_action_group_ : &fetch_action_group_;
        }

        // Conservative full flush of translated execution pages.
        void flushExecutionCache();

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

        // Hot-path loopback when ecache is active: bypasses fetch_()+Translate for
        // instructions on the same ExecutionPage. Routes to the active page's
        // translated_page_group_; on page exit that group redirects to fetch_action_group_
        // for a full re-translate.
        Action::ItrType execPageLoop_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup exec_page_loop_action_group_{"ExecPageLoop"};
    };
} // namespace pegasus
