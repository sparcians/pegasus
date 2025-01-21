#pragma once

#include <cinttypes>

namespace atlas
{
    namespace Sv32Pte
    {
        static constexpr uint32_t num_ppn_fields = 2;

        struct ppn1
        {
            static constexpr uint32_t bitmask = 0xfff00000;
            static constexpr uint32_t high_bit = 31;
            static constexpr uint32_t low_bit = 20;
        };

        struct ppn0
        {
            static constexpr uint32_t bitmask = 0x000ffc00;
            static constexpr uint32_t high_bit = 19;
            static constexpr uint32_t low_bit = 10;
        };

        struct rsw
        {
            static constexpr uint32_t bitmask = 0x00000300;
            static constexpr uint32_t high_bit = 9;
            static constexpr uint32_t low_bit = 8;
        };

        struct dirty
        {
            static constexpr uint32_t bitmask = 0x00000080;
            static constexpr uint32_t high_bit = 7;
            static constexpr uint32_t low_bit = 7;
        };

        struct accessed
        {
            static constexpr uint32_t bitmask = 0x00000040;
            static constexpr uint32_t high_bit = 6;
            static constexpr uint32_t low_bit = 6;
        };

        struct global
        {
            static constexpr uint32_t bitmask = 0x00000020;
            static constexpr uint32_t high_bit = 5;
            static constexpr uint32_t low_bit = 5;
        };

        struct user
        {
            static constexpr uint32_t bitmask = 0x00000010;
            static constexpr uint32_t high_bit = 4;
            static constexpr uint32_t low_bit = 4;
        };

        struct execute
        {
            static constexpr uint32_t bitmask = 0x00000008;
            static constexpr uint32_t high_bit = 3;
            static constexpr uint32_t low_bit = 3;
        };

        struct write
        {
            static constexpr uint32_t bitmask = 0x00000004;
            static constexpr uint32_t high_bit = 2;
            static constexpr uint32_t low_bit = 2;
        };

        struct read
        {
            static constexpr uint32_t bitmask = 0x00000002;
            static constexpr uint32_t high_bit = 1;
            static constexpr uint32_t low_bit = 1;
        };

        struct valid
        {
            static constexpr uint32_t bitmask = 0x00000001;
            static constexpr uint32_t high_bit = 0;
            static constexpr uint32_t low_bit = 0;
        };
    } // namespace Sv32Pte

    namespace Sv39Pte
    {
        static constexpr uint32_t num_ppn_fields = 3;

        struct ppn2
        {
            static constexpr uint64_t bitmask = 0x003fffff10000000;
            static constexpr uint32_t high_bit = 53;
            static constexpr uint32_t low_bit = 28;
        };

        struct ppn1
        {
            static constexpr uint64_t bitmask = 0x000000000ff80000;
            static constexpr uint32_t high_bit = 27;
            static constexpr uint32_t low_bit = 19;
        };

        struct ppn0
        {
            static constexpr uint64_t bitmask = 0x000000000007fc00;
            static constexpr uint32_t high_bit = 18;
            static constexpr uint32_t low_bit = 10;
        };

        struct rsw
        {
            static constexpr uint64_t bitmask = 0x0000000000000300;
            static constexpr uint32_t high_bit = 9;
            static constexpr uint32_t low_bit = 8;
        };

        struct dirty
        {
            static constexpr uint64_t bitmask = 0x0000000000000080;
            static constexpr uint32_t high_bit = 7;
            static constexpr uint32_t low_bit = 7;
        };

        struct accessed
        {
            static constexpr uint64_t bitmask = 0x0000000000000040;
            static constexpr uint32_t high_bit = 6;
            static constexpr uint32_t low_bit = 6;
        };

        struct global
        {
            static constexpr uint64_t bitmask = 0x0000000000000020;
            static constexpr uint32_t high_bit = 5;
            static constexpr uint32_t low_bit = 5;
        };

        struct user
        {
            static constexpr uint64_t bitmask = 0x0000000000000010;
            static constexpr uint32_t high_bit = 4;
            static constexpr uint32_t low_bit = 4;
        };

        struct execute
        {
            static constexpr uint64_t bitmask = 0x0000000000000008;
            static constexpr uint32_t high_bit = 3;
            static constexpr uint32_t low_bit = 3;
        };

        struct write
        {
            static constexpr uint64_t bitmask = 0x0000000000000004;
            static constexpr uint32_t high_bit = 2;
            static constexpr uint32_t low_bit = 2;
        };

        struct read
        {
            static constexpr uint64_t bitmask = 0x0000000000000002;
            static constexpr uint32_t high_bit = 1;
            static constexpr uint32_t low_bit = 1;
        };

        struct valid
        {
            static constexpr uint64_t bitmask = 0x00000001;
            static constexpr uint32_t high_bit = 0;
            static constexpr uint32_t low_bit = 0;
        };
    } // namespace Sv39Pte

    namespace Sv48Pte
    {
        static constexpr uint32_t num_ppn_fields = 4;

        struct ppn3
        {
            static constexpr uint64_t bitmask = 0x003fffe000000000;
            static constexpr uint32_t high_bit = 53;
            static constexpr uint32_t low_bit = 37;
        };

        struct ppn2
        {
            static constexpr uint64_t bitmask = 0x0000001ff0000000;
            static constexpr uint32_t high_bit = 36;
            static constexpr uint32_t low_bit = 28;
        };

        struct ppn1
        {
            static constexpr uint64_t bitmask = 0x000000000ff80000;
            static constexpr uint32_t high_bit = 27;
            static constexpr uint32_t low_bit = 19;
        };

        struct ppn0
        {
            static constexpr uint64_t bitmask = 0x000000000007fc00;
            static constexpr uint32_t high_bit = 18;
            static constexpr uint32_t low_bit = 10;
        };

        struct rsw
        {
            static constexpr uint64_t bitmask = 0x0000000000000300;
            static constexpr uint32_t high_bit = 9;
            static constexpr uint32_t low_bit = 8;
        };

        struct dirty
        {
            static constexpr uint64_t bitmask = 0x0000000000000080;
            static constexpr uint32_t high_bit = 7;
            static constexpr uint32_t low_bit = 7;
        };

        struct accessed
        {
            static constexpr uint64_t bitmask = 0x0000000000000040;
            static constexpr uint32_t high_bit = 6;
            static constexpr uint32_t low_bit = 6;
        };

        struct global
        {
            static constexpr uint64_t bitmask = 0x0000000000000020;
            static constexpr uint32_t high_bit = 5;
            static constexpr uint32_t low_bit = 5;
        };

        struct user
        {
            static constexpr uint64_t bitmask = 0x0000000000000010;
            static constexpr uint32_t high_bit = 4;
            static constexpr uint32_t low_bit = 4;
        };

        struct execute
        {
            static constexpr uint64_t bitmask = 0x0000000000000008;
            static constexpr uint32_t high_bit = 3;
            static constexpr uint32_t low_bit = 3;
        };

        struct write
        {
            static constexpr uint64_t bitmask = 0x0000000000000004;
            static constexpr uint32_t high_bit = 2;
            static constexpr uint32_t low_bit = 2;
        };

        struct read
        {
            static constexpr uint64_t bitmask = 0x0000000000000002;
            static constexpr uint32_t high_bit = 1;
            static constexpr uint32_t low_bit = 1;
        };

        struct valid
        {
            static constexpr uint64_t bitmask = 0x00000001;
            static constexpr uint32_t high_bit = 0;
            static constexpr uint32_t low_bit = 0;
        };
    } // namespace Sv48Pte

    namespace Sv57Pte
    {
        static constexpr uint32_t num_ppn_fields = 5;

        struct ppn4
        {
            static constexpr uint64_t bitmask = 0x003fc00000000000;
            static constexpr uint32_t high_bit = 53;
            static constexpr uint32_t low_bit = 46;
        };

        struct ppn3
        {
            static constexpr uint64_t bitmask = 0x00003fe000000000;
            static constexpr uint32_t high_bit = 45;
            static constexpr uint32_t low_bit = 37;
        };

        struct ppn2
        {
            static constexpr uint64_t bitmask = 0x0000001ff0000000;
            static constexpr uint32_t high_bit = 36;
            static constexpr uint32_t low_bit = 28;
        };

        struct ppn1
        {
            static constexpr uint64_t bitmask = 0x000000000ff80000;
            static constexpr uint32_t high_bit = 27;
            static constexpr uint32_t low_bit = 19;
        };

        struct ppn0
        {
            static constexpr uint64_t bitmask = 0x000000000007fc00;
            static constexpr uint32_t high_bit = 18;
            static constexpr uint32_t low_bit = 10;
        };

        struct rsw
        {
            static constexpr uint64_t bitmask = 0x0000000000000300;
            static constexpr uint32_t high_bit = 9;
            static constexpr uint32_t low_bit = 8;
        };

        struct dirty
        {
            static constexpr uint64_t bitmask = 0x0000000000000080;
            static constexpr uint32_t high_bit = 7;
            static constexpr uint32_t low_bit = 7;
        };

        struct accessed
        {
            static constexpr uint64_t bitmask = 0x0000000000000040;
            static constexpr uint32_t high_bit = 6;
            static constexpr uint32_t low_bit = 6;
        };

        struct global
        {
            static constexpr uint64_t bitmask = 0x0000000000000020;
            static constexpr uint32_t high_bit = 5;
            static constexpr uint32_t low_bit = 5;
        };

        struct user
        {
            static constexpr uint64_t bitmask = 0x0000000000000010;
            static constexpr uint32_t high_bit = 4;
            static constexpr uint32_t low_bit = 4;
        };

        struct execute
        {
            static constexpr uint64_t bitmask = 0x0000000000000008;
            static constexpr uint32_t high_bit = 3;
            static constexpr uint32_t low_bit = 3;
        };

        struct write
        {
            static constexpr uint64_t bitmask = 0x0000000000000004;
            static constexpr uint32_t high_bit = 2;
            static constexpr uint32_t low_bit = 2;
        };

        struct read
        {
            static constexpr uint64_t bitmask = 0x0000000000000002;
            static constexpr uint32_t high_bit = 1;
            static constexpr uint32_t low_bit = 1;
        };

        struct valid
        {
            static constexpr uint64_t bitmask = 0x00000001;
            static constexpr uint32_t high_bit = 0;
            static constexpr uint32_t low_bit = 0;
        };
    } // namespace Sv57Pte
} // namespace atlas
