#pragma once

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/ValidValue.hpp"

#include "core/PegasusCore.hpp"

namespace pegasus
{
    class ReservationMemory : public sparta::memory::BlockingMemoryIF
    {
        using addr_t = sparta::memory::addr_t;
        using Reservation = sparta::utils::ValidValue<addr_t>;

      public:
        ReservationMemory(PegasusCore* core, const std::string & desc, addr_t block_size,
                          addr_t total_size, sparta::memory::BlockingMemoryIF* memory_map) :
            BlockingMemoryIF(desc, block_size,
                             sparta::memory::DebugMemoryIF::AccessWindow(0, total_size)),
            core_(core),
            memory_map_(memory_map)
        {
        }

        virtual ~ReservationMemory() = default;

      private:
        virtual bool tryRead_(addr_t addr, addr_t size, uint8_t* buf,
                              const void* in_supplement = nullptr,
                              void* out_supplement = nullptr) override
        {
            return memory_map_->tryRead(addr, size, buf, in_supplement, out_supplement);
        }

        virtual bool tryWrite_(addr_t addr, addr_t size, const uint8_t* buf,
                               const void* in_supplement = nullptr,
                               void* out_supplement = nullptr) override
        {
            // FIXME: iterate through cores for multi-core support.
            for (uint32_t hart_id = 0; hart_id < core_->getNumThreads(); ++hart_id)
            {
                auto & resv = core_->getReservation(hart_id);
                if (resv.isValid() && resv == addr)
                {
                    core_->getPegasusState(hart_id)->storeOnReservationSet(true);
                }
            }
            return memory_map_->tryWrite(addr, size, buf, in_supplement, out_supplement);
        }

        virtual bool tryPeek_(addr_t addr, addr_t size, uint8_t* buf) const override
        {
            return memory_map_->tryPeek(addr, size, buf);
        }

      private:
        PegasusCore* core_ = nullptr;
        sparta::memory::BlockingMemoryIF* memory_map_ = nullptr;
    }; // class ReservationMemory
} // namespace pegasus