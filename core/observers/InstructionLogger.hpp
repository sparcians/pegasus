#pragma once

#include "core/observers/Observer.hpp"
#include "sparta/functional/Register.hpp"

namespace sparta::log
{
    class MessageSource;
}

namespace atlas
{
    class InstLogWriterBase;

    class InstructionLogger : public Observer
    {
      public:
        using base_type = InstructionLogger;

        InstructionLogger(sparta::log::MessageSource & inst_logger, const Observer::Arch arch);

        void useSpikeFormatting();

      private:
        void postExecute_(AtlasState*) override;

        sparta::log::MessageSource & inst_logger_;
        std::shared_ptr<InstLogWriterBase> inst_log_writer_;
    };
} // namespace atlas
