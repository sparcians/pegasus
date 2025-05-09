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
        Action* preExecute_(AtlasState*, Action*) override;
        Action* postExecute_(AtlasState*, Action*) override;
        Action* preException_(AtlasState*, Action*) override;

        sparta::log::MessageSource & inst_logger_;
        std::shared_ptr<InstLogWriterBase> inst_log_writer_;

        std::vector<uint8_t> getRegByteVector_(const sparta::Register* reg) const
        {
            const auto size = reg->getNumBits();
            if (size == 64)
            {
                return convertToByteVector(reg->dmiRead<uint64_t>());
            }
            else if (size == 32)
            {
                return convertToByteVector(reg->dmiRead<uint32_t>());
            }
            else
            {
                sparta_assert(false, "Unexpected register size, num bits: " << std::dec << size);
            }
        }

        uint64_t getRegValue_(const std::vector<uint8_t> & reg) const
        {
            const auto size = reg.size() * 8;
            if (size == 64)
            {
                return convertFromByteVector<uint64_t>(reg);
            }
            else if (size == 32)
            {
                return convertFromByteVector<uint32_t>(reg);
            }
            else
            {
                sparta_assert(false, "Unexpected register size, num bits: " << std::dec << size);
            }
        }
    };
} // namespace atlas
