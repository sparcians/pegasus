#include "system/MagicMemory.hpp"
#include "system/AtlasSystem.hpp"
#include "core/AtlasState.hpp"

namespace atlas
{
    MagicMemory::MagicMemory(sparta::TreeNode* node, const MagicMemoryParameters* params) :
        sparta::Unit(node),
        sparta::memory::BlockingMemoryIF("Magic Memory", AtlasSystem::ATLAS_SYSTEM_BLOCK_SIZE,
                                         {0, params->size, "magic_memory"}, nullptr),
        base_addr_(params->base_addr),
        size_(params->size),
        memory_(node, AtlasSystem::ATLAS_SYSTEM_BLOCK_SIZE, size_, 0)
    {
    }

    void MagicMemory::onBindTreeEarly_()
    {
        auto core_tn = getContainer()->getRoot()->getChildAs<sparta::ResourceTreeNode>("core0");
        state_ = core_tn->getResourceAs<AtlasState>();
    }

    bool MagicMemory::tryRead_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               uint8_t* buf, const void*, void*)
    {
        if (SPARTA_EXPECT_FALSE(debug_logger_))
        {
            debug_logger_ << std::hex << __func__ << " addr: 0x" << addr << " sz: " << std::dec
                          << size;
        }
        memory_.read(addr, size, buf);
        return true;
    }

    bool MagicMemory::tryWrite_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                                const uint8_t* buf, const void*, void*)
    {
        if (addr != 0)
        {
            if (SPARTA_EXPECT_FALSE(debug_logger_))
            {
                debug_logger_ << std::hex << __func__ << " addr: 0x" << addr << " sz: " << std::dec
                              << size;
            }
            memory_.write(addr, size, buf);
            return true;
        }

        MagicMemCommand mm_command;
        ::memcpy(mm_command.mm_buffer, buf, size);

        if (SPARTA_EXPECT_FALSE(info_logger_))
        {
            info_logger_ << __func__ << ": device: " << mm_command.cmd.device
                         << " command: " << mm_command.cmd.command
                         << " payload: " << mm_command.cmd.payload;
        }

        if (mm_command.cmd.payload == 0)
        {
            return true;
        }

        switch (static_cast<SupportedDevices>(mm_command.cmd.device))
        {
            case SupportedDevices::SYSCALL:
                {
                    if (mm_command.cmd.payload & 0x1)
                    {
                        const auto exit_code = mm_command.cmd.payload >> 1;
                        state_->stopSim(exit_code);
                    }
                    else
                    {
                        sparta_assert(false, "Magic memory command not supported: "
                                                 << std::dec << mm_command.cmd.command);
                    }
                }
                break;
            case SupportedDevices::BLOCK_CHAR:
                sparta_assert(false, "Block char is unsupported");
                break;
        }

        return true;
    }

    bool MagicMemory::tryPeek_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               uint8_t* buf) const
    {
        if (SPARTA_EXPECT_FALSE(debug_logger_))
        {
            debug_logger_ << std::hex << __func__ << " addr: 0x" << addr << " sz: " << std::dec
                          << size;
        }
        memory_.read(addr, size, buf);
        return true;
    }

    bool MagicMemory::tryPoke_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               const uint8_t* buf)
    {
        if (SPARTA_EXPECT_FALSE(debug_logger_))
        {
            debug_logger_ << std::hex << __func__ << " addr: 0x" << addr << " sz: " << std::dec
                          << size;
        }
        memory_.write(addr, size, buf);
        return true;
    }
} // namespace atlas
