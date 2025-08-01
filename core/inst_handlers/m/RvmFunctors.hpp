#pragma once

#include "include/PegasusUtils.hpp"

#include <stdint.h>
#include <bit>
#include <algorithm>
#include <functional>

namespace pegasus
{
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
} // namespace pegasus
