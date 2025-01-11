#pragma once

#include "core/observers/Observer.hpp"

#include "sparta/log/MessageSource.hpp"

namespace atlas
{
    class InstructionLogger : public Observer
    {
      public:
        using base_type = InstructionLogger;

        InstructionLogger(sparta::TreeNode* node);

        bool enabled() const override { return inst_logger_.observed(); }

      private:
        ActionGroup* preExecute_(AtlasState* state);
        ActionGroup* postExecute_(AtlasState* state);

        sparta::log::MessageSource inst_logger_;
    };
} // namespace atlas
