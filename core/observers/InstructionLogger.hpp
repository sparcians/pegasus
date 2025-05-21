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
        void preExecute_(AtlasState*) override;
        void postExecute_(AtlasState*) override;
        void preException_(AtlasState*) override;

        sparta::log::MessageSource & inst_logger_;
        std::shared_ptr<InstLogWriterBase> inst_log_writer_;
    };
} // namespace atlas
