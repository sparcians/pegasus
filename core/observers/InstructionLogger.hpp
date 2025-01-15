#pragma once

#include "core/observers/Observer.hpp"

#include "sparta/log/MessageSource.hpp"

namespace atlas
{
    class InstructionLogger : public Observer
    {
      public:
        using base_type = InstructionLogger;

        InstructionLogger(sparta::log::MessageSource& inst_logger);

      private:
        ActionGroup* preExecute_(AtlasState* state);
        ActionGroup* postExecute_(AtlasState* state);
        ActionGroup* preException_(AtlasState* state);

        sparta::log::MessageSource& inst_logger_;
    };
} // namespace atlas
