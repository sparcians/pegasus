#pragma once

#include "include/PegasusUtils.hpp"

#include <stdint.h>
#include <bit>
#include <algorithm>
#include <functional>

typedef int64_t sreg_t;
typedef uint64_t reg_t;

template <typename T> inline sreg_t sext32(T x) { return (sreg_t)(int32_t)(x); }

template <typename T> inline reg_t zext32(T x) { return (reg_t)(uint32_t)(x); }

template <typename T> inline sreg_t sext(T x, uint32_t pos)
{
    return (((sreg_t)(x) << (64 - (pos))) >> (64 - (pos)));
}

template <typename T> inline reg_t zext(T x, uint32_t pos)
{
    return (((reg_t)(x) << (64 - (pos))) >> (64 - (pos)));
}

template <typename T2, typename T1> inline T2 sext(T1 x)
{
    return static_cast<T2>(
        (static_cast<std::make_signed_t<T2>>(static_cast<std::make_signed_t<T1>>(x))));
}

template <typename T2, typename T1> inline T2 zext(T1 x)
{
    return static_cast<T2>(
        (static_cast<std::make_unsigned_t<T2>>(static_cast<std::make_unsigned_t<T1>>(x))));
}

inline uint64_t mulhu(uint64_t a, uint64_t b)
{
    uint64_t t = 0;
    uint32_t y1 = 0, y2 = 0;
    uint64_t a0 = (uint32_t)a, a1 = a >> 32;
    uint64_t b0 = (uint32_t)b, b1 = b >> 32;

    t = a1 * b0 + ((a0 * b0) >> 32);
    y1 = t;
    y2 = t >> 32;

    t = a0 * b1 + y1;

    t = a1 * b1 + y2 + (t >> 32);

    return t;
}

inline int64_t mulhsu(int64_t a, uint64_t b)
{
    int negate = a < 0;
    uint64_t res = mulhu(a < 0 ? -a : a, b);
    return negate ? ~res + (a * b == 0) : res;
}

inline int64_t mulh(int64_t a, int64_t b)
{
    int negate = (a < 0) != (b < 0);
    uint64_t res = mulhu(a < 0 ? -a : a, b < 0 ? -b : b);
    return negate ? ~res + ((uint64_t)a * (uint64_t)b == 0) : res;
}

template <typename T> struct Mulhu
{
    inline T operator()(const T & lhs, const T & rhs) const
    {
        if constexpr (sizeof(T) < sizeof(uint64_t))
        {
            uint64_t l{lhs};
            uint64_t r{rhs};
            return static_cast<T>((l * r) >> sizeof(T) * 8);
        }
        else
        {
            return mulhu(lhs, rhs);
        }
    }
};

template <typename T> struct Mulh
{
    inline T operator()(const T & lhs, const T & rhs) const
    {
        if constexpr (sizeof(T) < sizeof(uint64_t))
        {
            int64_t l{static_cast<std::make_signed_t<T>>(lhs)};
            int64_t r{static_cast<std::make_signed_t<T>>(rhs)};
            return static_cast<T>((l * r) >> sizeof(T) * 8);
        }
        else
        {
            return mulh(lhs, rhs);
        }
    }
};

template <typename T> struct Mulhsu
{
    inline T operator()(const T & lhs, const T & rhs) const
    {
        if constexpr (sizeof(T) < sizeof(uint64_t))
        {
            int64_t l{static_cast<std::make_signed_t<T>>(lhs)};
            uint64_t r{rhs};
            return static_cast<T>((l * r) >> sizeof(T) * 8);
        }
        else
        {
            return mulhsu(lhs, rhs);
        }
    }
};

template <typename T> struct Max
{
    inline const T & operator()(const T & lhs, const T & rhs) const { return std::max(lhs, rhs); }
};

template <typename T> struct Min
{
    inline const T & operator()(const T & lhs, const T & rhs) const { return std::min(lhs, rhs); }
};

template <typename T> struct Sll
{
    inline T operator()(const T & lhs, const T & rhs) const
    {
        return static_cast<T>(lhs << (rhs & (sizeof(T) * 8 - 1)));
    }
};

template <typename T> struct Srl
{
    inline T operator()(const T & lhs, const T & rhs) const
    {
        return static_cast<T>(lhs >> (rhs & (sizeof(T) * 8 - 1)));
    }
};

template <typename T> struct Sra
{
    inline T operator()(const T & lhs, const T & rhs) const
    {
        return static_cast<T>(static_cast<std::make_signed_t<T>>(lhs)
                              >> (rhs & (sizeof(T) * 8 - 1)));
    }
};

namespace pegasus
{
    template <uint64_t Mask> struct RegisterBitMask
    {
        static uint64_t mask(const uint64_t old_val, const uint64_t new_val)
        {
            // The 'Mask' template parameter is a bit mask that specifies which bits are writable.
            // We need to preserve the 'old_val' bits that are not writable, and replace the
            // writable bits with 'new_val'.
            return (old_val & ~Mask) | (new_val & Mask);
        }
    };

    template <> struct RegisterBitMask<0>;

    template <> struct RegisterBitMask<0xffffffffffffffff>
    {
        static uint64_t mask(const uint64_t old_val, const uint64_t new_val)
        {
            (void)old_val;
            return new_val;
        }
    };

    template <typename FieldT, typename Enable = void> struct CSRFields
    {
        static uint64_t readField(const uint64_t reg_val)
        {
            constexpr uint64_t num_field_bits = FieldT::high_bit - FieldT::low_bit + 1;
            constexpr uint64_t mask = (1 << num_field_bits) - 1;
            return (reg_val >> FieldT::low_bit) & mask;
        }
    };

} // namespace pegasus
