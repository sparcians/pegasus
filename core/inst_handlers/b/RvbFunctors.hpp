#pragma once

#include "include/PegasusTypes.hpp"

#include <bit>

namespace pegasus
{
    template <typename XLEN> struct Andn
    {
        XLEN operator()(XLEN rs1_val, XLEN rs2_val) const { return rs1_val & (~rs2_val); }
    };

    template <typename XLEN> struct CountlZero
    {
        XLEN operator()(XLEN rs1_val) const { return std::countl_zero(rs1_val); }
    };

    template <typename XLEN> struct Popcount
    {
        XLEN operator()(XLEN rs1_val) const { return std::popcount(rs1_val); }
    };

    template <typename XLEN> struct CountrZero
    {
        XLEN operator()(XLEN rs1_val) const { return std::countr_zero(rs1_val); }
    };

    template <typename XLEN> struct ByteSwap
    {
        XLEN operator()(XLEN rs1_val) const
        {
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return __builtin_bswap64(rs1_val);
            }
            else
                return __builtin_bswap32(rs1_val);
        }
    };

    template <typename S_XLEN> struct Max
    {
        S_XLEN operator()(S_XLEN rs1_val, S_XLEN rs2_val) const
        {
            return std::max(rs1_val, rs2_val);
        }
    };

    template <typename S_XLEN> struct Min
    {
        S_XLEN operator()(S_XLEN rs1_val, S_XLEN rs2_val) const
        {
            return std::min(rs1_val, rs2_val);
        }
    };

    template <typename XLEN> struct Orn
    {
        XLEN operator()(XLEN rs1_val, XLEN rs2_val) const { return rs1_val | (~rs2_val); }
    };

    template <typename XLEN> struct Rol
    {
        XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return std::rotl(rs1_val, rs2_val & (sizeof(XLEN) * 8 - 1));
        }
    };

    template <typename S_XLEN> struct Rolw
    {
        S_XLEN operator()(uint32_t rs1_val, uint32_t rs2_val) const
        {
            return static_cast<int32_t>(std::rotl(rs1_val, rs2_val & 0x1Full));
        }
    };

    template <typename XLEN> struct Ror
    {
        XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return std::rotr(rs1_val, rs2_val & (sizeof(XLEN) * 8 - 1));
        }
    };

    template <typename S_XLEN> struct Rorw
    {
        S_XLEN operator()(uint32_t rs1_val, uint32_t rs2_val) const
        {
            return static_cast<int32_t>(std::rotr(rs1_val, rs2_val & 0x1Full));
        }
    };

    template <typename XLEN, uint32_t x> struct SextX
    {
        XLEN operator()(XLEN rs1_val) const
        {
            return (((int64_t)(rs1_val) << (64 - (x))) >> (64 - (x)));
        }
    };

    template <typename XLEN> struct Xnor
    {
        XLEN operator()(XLEN rs1_val, XLEN rs2_val) const { return ~(rs1_val ^ rs2_val); }
    };

    template <typename XLEN> struct ZextH
    {
        XLEN operator()(XLEN rs1_val) const
        {
            return (((uint64_t)(rs1_val) << (64 - (16))) >> (64 - (16)));
        }
    };

    template <typename XLEN> struct Bclr
    {
        XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return rs1_val & ~(XLEN(1) << (rs2_val & (sizeof(XLEN) * 8 - 1)));
        }
    };

    template <typename XLEN> struct Bext
    {
        XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return (rs1_val >> (rs2_val & (sizeof(XLEN) * 8 - 1))) & XLEN(1);
        }
    };

    template <typename XLEN> struct Binv
    {
        XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return rs1_val ^ (XLEN(1) << (rs2_val & (sizeof(XLEN) * 8 - 1)));
        }
    };

    template <typename XLEN> struct Bset
    {
        XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return rs1_val | (XLEN(1) << (rs2_val & (sizeof(XLEN) * 8 - 1)));
        }
    };
} // namespace pegasus
