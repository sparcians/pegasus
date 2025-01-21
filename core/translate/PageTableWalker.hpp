//
// Created by skhan on 11/20/24.
//
#pragma once

#include "core/AtlasState.hpp"

namespace atlas
{
    // TODO: A template class based on Mode, all methods will work according to the mode, hence
    // methods will also be template
    class PageTableWalker
    {

        const std::vector<uint32_t> offsets_{12, 12, 12};
        const std::vector<std::vector<uint32_t>> vpn_ = {
            {10, 10},    // Sv32
            {9, 9, 9},   // Sv39
            {9, 9, 9, 9} // Sv48
        };

        inline uint32_t getMMUModeOffset(MMUMode mmu_mode)
        {
            return offsets_.at(static_cast<uint32_t>(mmu_mode));
        }

        inline uint32_t getMMUModeVPNBits(MMUMode mmu_mode, int index)
        {
            const auto & vpn_bits = vpn_.at(static_cast<uint32_t>(mmu_mode));
            if (index >= ((int)vpn_bits.size()))
            {
                throw std::out_of_range("VPN index out of range for MMU mode");
            }
            return vpn_bits[index];
        }

        std::vector<uint32_t> getMMUModeVPNs(MMUMode mmu_mode)
        {
            return vpn_.at(static_cast<uint32_t>(mmu_mode));
        }

        uint32_t extractOffset(uint32_t virtualAddress, MMUMode mode)
        {
            uint32_t offsetBitsSize = getMMUModeOffset(mode);
            return virtualAddress
                   & ((1 << offsetBitsSize) - 1); // Mask to extract the lower 'offsetBitsSize' bits
        }

        std::vector<uint32_t> extractVPN(uint32_t virtualAddress, MMUMode mode)
        {
            std::vector<uint32_t> vpnValues;
            std::vector<uint32_t> vpn_bits = getMMUModeVPNs(mode);
            uint32_t shift = getMMUModeOffset(mode);
            for (uint32_t bits : vpn_bits)
            {
                vpnValues.push_back((virtualAddress >> shift) & ((1 << bits) - 1));
                shift += bits;
            }
            return vpnValues;
        }

      public:
        PageTableWalker() {}

        uint32_t getPFN(uint32_t, AtlasState*);
        uint32_t sv32PageTableWalk(uint32_t, uint32_t, AtlasState*);
    };

} // namespace atlas
