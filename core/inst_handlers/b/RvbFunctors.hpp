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

    template <typename XLEN> struct Pack
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            constexpr XLEN mask = ((XLEN)1 << (sizeof(XLEN) << 2)) - 1;
            const XLEN lo = rs1_val & mask;
            const XLEN hi = rs2_val << (sizeof(XLEN) << 2);

            return hi | lo;
        }
    };

    template <typename XLEN> struct Packh
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            const XLEN lo = rs1_val & ((XLEN) 0xff);
            const XLEN hi = (rs2_val & ((XLEN) 0xff)) << 8;

            return hi | lo;
        }
    };

    template <typename XLEN> struct Packw
    {
        inline XLEN operator()(XLEN rs1_val, XLEN rs2_val) const
        {
            // Due to sign extension, only support RV64
            static_assert(std::is_same_v<XLEN, RV64>);

            const XLEN lo = rs1_val & ((XLEN) 0xffff);
            const XLEN hi = (rs2_val & ((XLEN) 0xffff)) << 16;

            return (XLEN)(int64_t)(int32_t)(hi | lo);
        }
    };

    template <typename XLEN> struct Brev8
    {
        inline XLEN operator()(XLEN rs1_val) const
        {
            using U = std::make_unsigned_t<XLEN>;
            const U rs1 = static_cast<U> (rs1_val);
            U rd = 0;

            // bit-reverse a signle byte
            constexpr auto rev8 = [] (std::uint8_t b) -> std::uint8_t {
                b = (std::uint8_t) ((b >> 4) | (b << 4));  // 3210_7654
                b = (std::uint8_t) (((b & 0xCCu) >> 2) | ((b & 0x33u) << 2));  // 1032_5476
                b = (std::uint8_t) (((b & 0xAAu) >> 1) | ((b & 0x55u) << 1));  // 0123_4567
                return b;
            };

            for (unsigned i = 0; i < sizeof(U); ++i) {
                std::uint8_t b = static_cast<std::uint8_t>(rs1 >> (i * 8));
                rd |= static_cast<U>(static_cast<U>(rev8(b)) << (i * 8));
            }

            return static_cast<XLEN>(rd);
        }
    };

    template <typename XLEN> struct Unzip
    {
      inline XLEN operator()(XLEN rs1_val) const
      {
          using U = std::make_unsigned_t<XLEN>;
          constexpr unsigned num_bits = sizeof(U) * 8 / 2;

          const U rs1 = static_cast<U>(rs1_val);
          U lo = 0;  // packed even bits
          U hi = 0;  // packed odd  bits

          for (unsigned i = 0; i < num_bits; ++i) {
              lo |= ((rs1 >> (2 * i))     & 1u) << i;
              hi |= ((rs1 >> (2 * i + 1)) & 1u) << i;
          }

          return static_cast<XLEN>(lo | (hi << num_bits));
      }
    };

    template <typename XLEN> struct Zip
    {
      inline XLEN operator()(XLEN rs1_val) const
      {
          using U = std::make_unsigned_t<XLEN>;
          constexpr unsigned num_bits = sizeof(U) * 8 / 2;

          constexpr U mask = ((U) 1 << num_bits) - 1;
          const U rs1 = static_cast<U>(rs1_val);

          const U lo  =  rs1               & mask;  // packed even bits
          const U hi  = (rs1 >> num_bits)  & mask;  // packed odd  bits

          U out = 0;
          for (unsigned i = 0; i < num_bits; ++i) {
              out |= ((lo >> i) & 1u) << (2 * i);       // even positions
              out |= ((hi >> i) & 1u) << (2 * i + 1);   // odd positions
          }

          return static_cast<XLEN>(out);
      }
    };

} // namespace pegasus
