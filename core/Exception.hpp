#pragma once

#include "core/ActionGroup.hpp"
#include "core/Trap.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/Unit.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{

    class AtlasState;

    /// This class handles exceptions that occur during the fetch-translate-decode-execute cycle,
    /// e.g. for handling illegal instructions.
    class Exception : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "Exception";
        using base_type = Exception;

        class ExceptionParameters : public sparta::ParameterSet
        {
          public:
            ExceptionParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}
        };

        Exception(sparta::TreeNode* exception_node, const ExceptionParameters* p);

        void setUnhandledException(const TrapCauses cause) { cause_ = cause; }

        const sparta::utils::ValidValue<TrapCauses> & getUnhandledException() const
        {
            return cause_;
        }

        ActionGroup* getActionGroup() { return &exception_action_group_; }

      private:
        void onBindTreeEarly_() override;

        ActionGroup* handleException_(atlas::AtlasState* state);

        ActionGroup exception_action_group_{"Exception"};

        void handleUModeException_(atlas::AtlasState* state);

        void handleSModeException_(atlas::AtlasState* state);

        void handleMModeException_(atlas::AtlasState* state);

        sparta::utils::ValidValue<TrapCauses> cause_;
    };

} // namespace atlas
