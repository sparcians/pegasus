#pragma once

#include "include/PegasusUtils.hpp"

#include <stdint.h>
#include <bit>
#include <algorithm>
#include <functional>

namespace pegasus
{
    template <typename T> struct Max
    {
        inline const T & operator()(const T & lhs, const T & rhs) const
        {
            return std::max(lhs, rhs);
        }
    };

    template <typename T> struct Min
    {
        inline const T & operator()(const T & lhs, const T & rhs) const
        {
            return std::min(lhs, rhs);
        }
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
} // namespace pegasus
