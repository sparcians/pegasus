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
