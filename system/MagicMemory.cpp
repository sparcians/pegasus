
#include "system/MagicMemory.hpp"
#include "system/AtlasSystem.hpp"

namespace atlas
{
    MagicMemory::MagicMemory(sparta::TreeNode* node, sparta::memory::addr_t base_addr,
                             sparta::memory::addr_t size) :
        sparta::Unit(node),
        sparta::memory::BlockingMemoryIF("Magic Memory", AtlasSystem::ATLAS_SYSTEM_BLOCK_SIZE,
                                         {0, size, "magic_memory"}, nullptr),
        base_addr_(base_addr),
        size_(size)
    {
    }

    bool MagicMemory::tryRead_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               uint8_t* buf, const void*, void*)
    {
        (void)addr;
        (void)size;
        *buf = 0x60;
        return true;
    }

    bool MagicMemory::tryWrite_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                                const uint8_t* buf, const void*, void*)
    {
        (void)addr;
        (void)size;
        std::cout << *buf;
        return true;
    }

    bool MagicMemory::tryPeek_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               uint8_t* buf) const
    {
        (void)addr;
        (void)buf;
        sparta_assert(false, "Simple UART peeks are not implemented");
        return false;
    }

    bool MagicMemory::tryPoke_(sparta::memory::addr_t addr, sparta::memory::addr_t size,
                               const uint8_t* buf)
    {
        (void)addr;
        (void)buf;
        sparta_assert(false, "Simple UART pokes are not implemented");
        return false;
    }
} // namespace atlas
