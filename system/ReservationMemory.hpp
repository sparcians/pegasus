#pragma once

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/ValidValue.hpp"

#include "core/PegasusCore.hpp"

namespace pegasus
{
    class ReservationMemory : public sparta::memory::SimpleMemoryMapNode
    {
        using addr_t = sparta::memory::addr_t;
        using Reservation = sparta::utils::ValidValue<addr_t>;

      public:
        ReservationMemory(sparta::TreeNode* parent, const std::string & name, const std::string & group,
                   group_idx_type group_idx, const std::string & desc, addr_t block_size,
                   addr_t total_size, sparta::memory::TranslationIF* transif = nullptr) :
            SimpleMemoryMapNode(parent, name, group, group_idx, desc, block_size, total_size,
                                transif)
        {
        }

      protected:
        virtual bool tryWrite(addr_t addr, addr_t size, const uint8_t* buf,
                              const void* in_supplement = nullptr,
                              void* out_supplement = nullptr) override
        {
            // It seems we currely only support 1 core, see "MagicMemory::onBindTreeEarly_()"
            auto core = getRoot()->getChild("core0")->getResourceAs<PegasusCore*>();
            for (uint32_t hart_id = 0; hart_id < core->getNumThreads(); ++hart_id)
            {
                auto resv = core->getReservation(hart_id);
                if (resv.isValid() && resv == addr)
                {
                    core->getPegasusState(hart_id)->storeOnReservationSet(true);
                }
            }
            return SimpleMemoryMapNode::tryWrite(addr, size, buf, in_supplement, out_supplement);
        }
    }; // class ReservationMemory
} // namespace pegasus