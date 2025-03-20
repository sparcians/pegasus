
#include "system/MagicMemory.hpp"
#include "system/AtlasSystem.hpp"

namespace atlas
{
    MagicMemory::MagicMemory(sparta::TreeNode* node, const MagicMemoryParameters* params) :
        sparta::Unit(node),
        sparta::memory::BlockingMemoryIF("Magic Memory", AtlasSystem::ATLAS_SYSTEM_BLOCK_SIZE,
                                         {0, params->size, "magic_memory"}, nullptr),
        info_logger_(node, "info", "magic memory info logger"),
        debug_logger_(node, "debug", "magic memory debug logger"),
        base_addr_(params->base_addr),
        size_(params->size),
        memory_(node, AtlasSystem::ATLAS_SYSTEM_BLOCK_SIZE, size_, 0)
    {
    }

    bool MagicMemory::tryRead_(sparta::memory::addr_t addr, sparta::memory::addr_t size, uint8_t* buf,
                               const void*, void*)
    {
        if(SPARTA_EXPECT_FALSE(debug_logger_)) {
            debug_logger_ << std::hex << __func__ << " addr: 0x" << addr << " sz: " << std::dec << size;
        }
        memory_.read(addr, size, buf);
        return true;
    }

    bool MagicMemory::tryWrite_(sparta::memory::addr_t addr, sparta::memory::addr_t size, const uint8_t* buf,
                                const void*, void*)
    {
        if (addr != 0)
        {
            if(SPARTA_EXPECT_FALSE(debug_logger_)) {
                debug_logger_ << std::hex << __func__ << " addr: 0x" << addr << " sz: " << std::dec << size;
            }
            memory_.write(addr, size, buf);
            return true;
        }

        MagicMemCommand mm_command;
        ::memcpy(mm_command.mm_buffer, buf, size);

        if(SPARTA_EXPECT_FALSE(info_logger_)) {
            info_logger_ << __func__
                         << ": device: " << mm_command.cmd.device
                         << " command: " << mm_command.cmd.command
                         << " payload: " << mm_command.cmd.payload;
        }

        if (mm_command.cmd.payload == 0) { return true; }

        switch(static_cast<SupportedDevices>(mm_command.cmd.device))
        {
            case SupportedDevices::SYSCALL:
            {
                //SysCallEmulator::SysCallStack stack;

                if (SPARTA_EXPECT_FALSE(mm_command.cmd.payload & 0x1)) {
                    const auto exit_code = mm_command.cmd.payload >> 1;
                    //syscall_emulator_->exitCall(exit_code);
                } else {
                    /*
                    // Check if stack address crosses memory blocks
                    const sparta::memory::addr_t stack_addr = mm_command.cmd.payload;
                    const sparta::memory::addr_t block_size = memory_map_->getBlockSize();
                    const sparta::memory::addr_t block_mask = block_size - 1;
                    const sparta::memory::addr_t first_read_size = std::min(static_cast<sparta::memory::addr_t>(sizeof(stack)),
                                                                            block_size - (stack_addr & block_mask));
                    // Read the stack information
                    memory_map_->read(stack_addr, first_read_size,
                                      reinterpret_cast<uint8_t*>(&stack[0]), nullptr, nullptr);
                    // Read again if the last read is partial
                    if (SPARTA_EXPECT_FALSE(first_read_size < sizeof(stack))) {
                        const sparta::memory::addr_t second_read_size = sizeof(stack) - first_read_size;
                        memory_map_->read(stack_addr + first_read_size, second_read_size,
                                          reinterpret_cast<uint8_t*>(&stack[0]) + first_read_size, nullptr, nullptr);
                    }

                    // Perform the system call
                    auto sys_ret = syscall_emulator_->serviceCall(stack, memory_map_);

                    if(SPARTA_EXPECT_FALSE(info_logger_)) {
                        info_logger_ << std::hex << __func__
                            << " syscall: " << stack[0]
                            << " ret: " << sys_ret;
                    }

                    // Clear tohost
                    uint64_t zero = 0;
                    memory_.write(tohost_addr_offset_, sizeof(uint64_t),
                                          reinterpret_cast<uint8_t*>(&zero));

                    // Update fromhost with a response
                    uint64_t fromhost_response =
                        ((mm_command.tohost_data >> 48) << 48) |
                        ((sys_ret << 16) >> 16);
                    memory_.write(fromhost_addr_offset_, sizeof(fromhost_response),
                                          reinterpret_cast<uint8_t*>(&fromhost_response));
                    */
                }
            }
            break;
            case SupportedDevices::BLOCK_CHAR:
                sparta_assert(false, "Block char is unsupported");
                break;
        }

        return true;
    }

    bool MagicMemory::tryPeek_(sparta::memory::addr_t addr, sparta::memory::addr_t size, uint8_t* buf) const
    {
        if(SPARTA_EXPECT_FALSE(debug_logger_)) {
            debug_logger_ << std::hex << __func__ << " addr: 0x" << addr << " sz: " << std::dec << size;
        }
        memory_.read(addr, size, buf);
        return true;
    }

    bool MagicMemory::tryPoke_(sparta::memory::addr_t addr, sparta::memory::addr_t size, const uint8_t* buf)
    {
        if(SPARTA_EXPECT_FALSE(debug_logger_)) {
            debug_logger_ << std::hex << __func__ << " addr: 0x" << addr << " sz: " << std::dec << size;
        }
        memory_.write(addr, size, buf);
        return true;
    }
} // namespace atlas
