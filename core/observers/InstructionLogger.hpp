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

    template <typename XLEN> class InstructionLogger : public Observer
    {
      public:
        using base_type = InstructionLogger<XLEN>;

        InstructionLogger(sparta::log::MessageSource & inst_logger);

        void useSpikeFormatting();

      private:
        ActionGroup* preExecute_(AtlasState* state) override;
        ActionGroup* postExecute_(AtlasState* state) override;
        ActionGroup* preException_(AtlasState* state) override;

        sparta::log::MessageSource & inst_logger_;
        std::shared_ptr<InstLogWriterBase> inst_log_writer_;
    };
} // namespace atlas
