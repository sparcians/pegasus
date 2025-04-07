#pragma once

#include "core/observers/Observer.hpp"
#include "core/MessageLogger.hpp"
#include "sparta/functional/Register.hpp"

namespace atlas
{
    // TODO cnyce
    enum class InstLogFormat
    {
        ATLAS, SPIKE
    };

    class InstructionLogger : public Observer
    {
      public:
        using base_type = InstructionLogger;

        InstructionLogger(const size_t xlen = 64,
                          const std::string& filename = "inst_log.out",
                          const InstLogFormat format = InstLogFormat::ATLAS);

      private:
        ActionGroup* preExecute_(AtlasState* state) override;
        ActionGroup* postExecute_(AtlasState* state) override;
        ActionGroup* preException_(AtlasState* state) override;

        //! Format-specific postExecute_()
        void atlasPostExecute_(AtlasState* state);
        void spikePostExecute_(AtlasState* state);

        //! XLEN: 64 or 32 bit
        const size_t xlen_;

        //! Format: Atlas or Spike
        const InstLogFormat format_;

        //! Instruction logger
        MessageLogger inst_logger_;

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
