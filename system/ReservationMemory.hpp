#pragma once

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/ValidValue.hpp"

#include "core/PegasusCore.hpp"

namespace pegasus
{
    class ReservationMemory : public sparta::memory::BlockingMemoryIFNode
    {
        using addr_t = sparta::memory::addr_t;
        using Reservation = sparta::utils::ValidValue<addr_t>;

      public:
        ReservationMemory(sparta::TreeNode* parent, const std::string & name,
                          const std::string & group, group_idx_type group_idx,
                          const std::string & desc, addr_t block_size, addr_t total_size) :
            BlockingMemoryIFNode(parent, name, group, group_idx, desc, block_size,
                                 sparta::memory::DebugMemoryIF::AccessWindow(0, total_size)),
            memory_map_(new sparta::memory::SimpleMemoryMapNode(
                parent, "memory_map", group, group_idx, "Pegasus System Memory Map", block_size,
                total_size))
        {
        }

        virtual ~ReservationMemory() { delete memory_map_; }

        sparta::memory::SimpleMemoryMapNode* getMemoryMap() { return memory_map_; }

      protected:
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
            auto core = getRoot()->getChild("core0")->getResourceAs<PegasusCore*>();
            for (uint32_t hart_id = 0; hart_id < core->getNumThreads(); ++hart_id)
            {
                auto resv = core->getReservation(hart_id);
                if (resv.isValid() && resv == addr)
                {
                    core->getPegasusState(hart_id)->storeOnReservationSet(true);
                }
            }
            return memory_map_->tryWrite(addr, size, buf, in_supplement, out_supplement);
        }

        virtual bool tryPeek_(addr_t addr, addr_t size, uint8_t* buf) const override
        {
            return memory_map_->tryPeek(addr, size, buf);
        }

      private:
        sparta::memory::SimpleMemoryMapNode* memory_map_ = nullptr;
    }; // class ReservationMemory
} // namespace pegasus