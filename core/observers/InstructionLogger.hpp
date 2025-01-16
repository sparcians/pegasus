#pragma once

#include "core/observers/Observer.hpp"

#include "sparta/log/MessageSource.hpp"

namespace atlas
{
    class InstructionLogger : public Observer
    {
      public:
        using base_type = InstructionLogger;

        InstructionLogger(sparta::log::MessageSource & inst_logger);

        ActionGroup* preExecute(AtlasState* state) override;
        ActionGroup* postExecute(AtlasState* state) override;
        ActionGroup* preException(AtlasState* state) override;

      private:
        sparta::log::MessageSource & inst_logger_;
    };
} // namespace atlas
