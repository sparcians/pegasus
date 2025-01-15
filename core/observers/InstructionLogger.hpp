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

        void preExecute(AtlasState* state) override;
        void postExecute(AtlasState* state) override;
        void preException(AtlasState* state) override;

      private:
        sparta::log::MessageSource& inst_logger_;
    };
} // namespace atlas
