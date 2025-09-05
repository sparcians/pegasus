#pragma once

#include "include/PegasusUtils.hpp"

#include <limits>
#include <stdint.h>

namespace pegasus
{
    template <typename T>
    concept IsSigned = std::is_signed_v<T>;

    template <typename T> struct AddSat
    {
        inline T operator()(const T & x, const T & y, bool & sat) const
        requires IsSigned<T>
        {
            using UT = std::make_unsigned_t<T>;
            UT ux = x;
            UT uy = y;
            UT res = ux + uy;
            sat = false;
            constexpr size_t sh = sizeof(T) * 8 - 1;

            ux = (ux >> sh) + ((UT{0x1} << sh) - 1);
            if (static_cast<T>((ux ^ uy) | ~(uy ^ res)) >= 0)
            {
                res = ux;
                sat = true;
            }

            return res;
        }

        inline T operator()(const T & x, const T & y, bool & sat) const
        requires(!IsSigned<T>)
        {
            sat = false;
            T res = x + y;

            sat = res < x;
            res |= -(res < x);

            return res;
        }
    };

    template <typename T> struct SubSat
    {
        inline T operator()(const T & x, const T & y, bool & sat) const
        requires IsSigned<T>
        {
            using UT = std::make_unsigned_t<T>;
            UT ux = x;
            UT uy = y;
            UT res = ux + uy;
            sat = false;
            constexpr size_t sh = sizeof(T) * 8 - 1;

            ux = (ux >> sh) + ((UT{0x1} << sh) - 1);
            if (static_cast<T>((ux ^ uy) & (ux ^ res)) < 0)
            {
                res = ux;
                sat = true;
            }

            return res;
        }

        inline T operator()(const T & x, const T & y, bool & sat) const
        requires(!IsSigned<T>)
        {
            sat = false;
            T res = x - y;

            sat = res > x;
            res &= -(res <= x);

            return res;
        }
    };
} // namespace pegasus
