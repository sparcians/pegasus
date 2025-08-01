#pragma once

#include "include/PegasusTypes.hpp"

#include <bit>

namespace pegasus
{
    template <typename XLEN> struct Andn
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const { return rs1_val & (~rs2_val); }
    };

    template <typename XLEN> struct CountlZero
    {
        inline XLEN operator()(XLEN rs1_val) const { return std::countl_zero(rs1_val); }
    };

    template <typename XLEN> struct Popcount
    {
        inline XLEN operator()(XLEN rs1_val) const { return std::popcount(rs1_val); }
    };

    template <typename XLEN> struct CountrZero
    {
        inline XLEN operator()(XLEN rs1_val) const { return std::countr_zero(rs1_val); }
    };

    template <typename XLEN> struct ByteSwap
    {
        inline XLEN operator()(XLEN rs1_val) const
        {
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return __builtin_bswap64(rs1_val);
            }
            else
                return __builtin_bswap32(rs1_val);
        }
    };

    template <typename XLEN> struct Orn
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const { return rs1_val | (~rs2_val); }
    };

    template <typename XLEN> struct Rol
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return std::rotl(rs1_val, rs2_val);
        }
    };

    template <typename S_XLEN> struct Rolw
    {
        inline S_XLEN operator()(uint32_t rs1_val, uint32_t rs2_val) const
        {
            return static_cast<int32_t>(std::rotl(rs1_val, rs2_val & 0x1Full));
        }
    };

    template <typename XLEN> struct Ror
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return std::rotr(rs1_val, rs2_val & (sizeof(XLEN) * 8 - 1));
        }
    };

    template <typename S_XLEN> struct Rorw
    {
        inline S_XLEN operator()(uint32_t rs1_val, uint32_t rs2_val) const
        {
            return static_cast<int32_t>(std::rotr(rs1_val, rs2_val & 0x1Full));
        }
    };

    template <typename XLEN, uint32_t X> struct SextX
    {
        inline XLEN operator()(XLEN rs1_val) const
        {
            return (((int64_t)(rs1_val) << (64 - X)) >> (64 - X));
        }
    };

    template <typename XLEN> struct Xnor
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const { return ~(rs1_val ^ rs2_val); }
    };

    template <typename XLEN> struct ZextH
    {
        inline XLEN operator()(XLEN rs1_val) const
        {
            return (((uint64_t)(rs1_val) << (64 - (16))) >> (64 - (16)));
        }
    };

    template <typename XLEN> struct Bclr
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return rs1_val & ~(XLEN(1) << (rs2_val & (sizeof(XLEN) * 8 - 1)));
        }
    };

    template <typename XLEN> struct Bext
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return (rs1_val >> (rs2_val & (sizeof(XLEN) * 8 - 1))) & XLEN(1);
        }
    };

    template <typename XLEN> struct Binv
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return rs1_val ^ (XLEN(1) << (rs2_val & (sizeof(XLEN) * 8 - 1)));
        }
    };

    template <typename XLEN> struct Bset
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            return rs1_val | (XLEN(1) << (rs2_val & (sizeof(XLEN) * 8 - 1)));
        }
    };
} // namespace pegasus
