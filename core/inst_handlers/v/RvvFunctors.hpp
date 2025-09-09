#pragma once

#include "include/PegasusUtils.hpp"
#include "core/inst_handlers/m/RvmFunctors.hpp"

#include <limits>
#include <stdint.h>

namespace pegasus
{
    template <typename T>
    concept IsSigned = std::is_signed_v<T>;

    bool sat = false; // Global flag for saturation

    enum class Xrm
    {
        RNU, // round-to-nearest-up
        RNE, // round-to-nearest-even
        RDN, // round-down (truncate)
        ROD  // round-to-odd (OR bits into LSB, aka "jam")
    } xrm;   // Glocal flag for fixed-point rounding mode

    template <typename T> struct SatAdd
    {
        inline T operator()(const T & x, const T & y) const
        requires IsSigned<T>
        {
            using UT = std::make_unsigned_t<T>;
            UT ux = x;
            UT uy = y;
            UT res = ux + uy;
            constexpr size_t sh = sizeof(T) * 8 - 1;

            ux = (ux >> sh) + ((UT{0x1} << sh) - 1);
            if (static_cast<T>((ux ^ uy) | ~(uy ^ res)) >= 0)
            {
                res = ux;
                sat = true;
            }

            return res;
        }

        inline T operator()(const T & x, const T & y) const
        requires(!IsSigned<T>)
        {
            T res = x + y;

            sat |= res < x;
            res |= -(res < x);

            return res;
        }
    };

    template <typename T> struct SatSub
    {
        inline T operator()(const T & x, const T & y) const
        requires IsSigned<T>
        {
            using UT = std::make_unsigned_t<T>;
            UT ux = x;
            UT uy = y;
            UT res = ux - uy;
            constexpr size_t sh = sizeof(T) * 8 - 1;

            ux = (ux >> sh) + ((UT{0x1} << sh) - 1);
            if (static_cast<T>((ux ^ uy) & (ux ^ res)) < 0)
            {
                res = ux;
                sat = true;
            }

            return res;
        }

        inline T operator()(const T & x, const T & y) const
        requires(!IsSigned<T>)
        {
            T res = x - y;

            sat |= res > x;
            res &= -(res <= x);

            return res;
        }
    };

    template <typename T> T intRounding(T result, bool & carry, size_t gb)
    {
        const T lsb = T{1} << (gb);
        const T lsb_half = lsb >> 1;
        T res = result;
        switch (xrm)
        {
            case Xrm::RNU:
                res = result + lsb_half;
                break;
            case Xrm::RNE:
                if ((result & lsb_half) && ((result & (lsb_half - 1)) || (result & lsb)))
                {
                    res = result + lsb;
                }
                break;
            case Xrm::RDN:
                break;
            case Xrm::ROD:
                if (result & (lsb - 1))
                {
                    res = result | lsb;
                }
                break;
            default:
                break;
        }
        carry ^= res < result;
        return res;
    }

    template <typename T> struct AveAdd
    {
        inline T operator()(const T & x, const T & y) const
        {
            using UT = std::make_unsigned_t<T>;
            T res = x + y;
            bool c = (x < 0) ^ (y < 0) ^ (static_cast<UT>(res) < static_cast<UT>(x));
            res = intRounding(res, c, 1);
            return (static_cast<UT>(res) >> 1) | (T{c} << (sizeof(T) * 8 - 1));
        }
    };

    template <typename T> struct AveSub
    {
        inline T operator()(const T & x, const T & y) const
        {
            using UT = std::make_unsigned_t<T>;
            T res = x - y;
            bool c = (x < 0) ^ (y < 0) ^ (static_cast<UT>(res) > static_cast<UT>(x));
            res = intRounding(res, c, 1);
            return (static_cast<UT>(res) >> 1) | (T{c} << (sizeof(T) * 8 - 1));
        }
    };

    template <typename T> struct SatMul
    {
        inline T operator()(const T & x, const T & y) const
        {
            using UT = std::make_unsigned_t<T>;

            if ((x == y) && (x == std::numeric_limits<T>::min()))
            {
                sat = true;
                return std::numeric_limits<T>::max();
            }

            T res_l = x * y;
            T res_h = mulh(x, y);

            // rounding
            bool c = false;
            res_l = intRounding(res_l, c, sizeof(T) * 8 - 1);
            res_h += c;

            return (static_cast<UT>(res_l) >> (sizeof(T) * 8 - 1)) | (res_h << 1);
        }
    };
} // namespace pegasus
