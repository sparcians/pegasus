#pragma once

#include "include/PegasusUtils.hpp"

#include <stdint.h>
#include <bit>
#include <algorithm>
#include <functional>

namespace pegasus
{
    template <typename T> struct Swap
    {
        constexpr T operator()(const T & lhs, const T & rhs) const
        {
            (void)lhs;
            return rhs;
        }
    };
} // namespace pegasus
