
#include "system/MagicMemory.hpp"
#include "system/AtlasSystem.hpp"

namespace atlas
{
    MagicMemory::MagicMemory(sparta::TreeNode* node, const MagicMemoryParameters* params) :
        sparta::Unit(node),
        sparta::memory::BlockingMemoryIF("Magic Memory", AtlasSystem::ATLAS_SYSTEM_BLOCK_SIZE,
                                         {0, params->size, "magic_memory"}, nullptr),
        base_addr_(params->base_addr),
        size_(params->size)
    {
    }

    bool MagicMemory::tryRead_(sparta::memory::addr_t, sparta::memory::addr_t, uint8_t*,
                               const void*, void*)
    {
        // TODO: Implement magic memory reads
        return true;
    }

    bool MagicMemory::tryWrite_(sparta::memory::addr_t, sparta::memory::addr_t, const uint8_t*,
                                const void*, void*)
    {
        // TODO: Implement magic memory writes
        return true;
    }

    bool MagicMemory::tryPeek_(sparta::memory::addr_t, sparta::memory::addr_t, uint8_t*) const
    {
        // Peeks are allowed, but have no side effects
        return true;
    }

    bool MagicMemory::tryPoke_(sparta::memory::addr_t, sparta::memory::addr_t, const uint8_t*)
    {
        // Pokes are allowed, but have no side effects
        return true;
    }
} // namespace atlas
