#pragma once

#include "include/PegasusUtils.hpp"
#include "core/inst_handlers/m/RvmFunctors.hpp"

#include <limits>
#include <stdint.h>

namespace pegasus
{
    // Global flag for saturation
    bool sat = false;

    // Global flag for fixed-point rounding mode
    enum class Xrm
    {
        RNU, // round-to-nearest-up
        RNE, // round-to-nearest-even
        RDN, // round-down (truncate)
        ROD  // round-to-odd (OR bits into LSB, aka "jam")
    } xrm;

    // Fixed-point

    template <typename T> T intRounding(T result, bool & carry, size_t gb)
    {
        using UT = std::make_unsigned_t<T>;
        const UT lsb = UT{1} << (gb);
        const UT lsb_half = lsb >> 1;
        UT res = result;
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
        carry ^= (res < static_cast<UT>(result));
        return static_cast<T>(res);
    }

    template <typename T> struct SatAdd
    {
        inline T operator()(const T & x, const T & y) const
        {
            using UT = std::make_unsigned_t<T>;
            UT ux = x;
            UT uy = y;
            UT res = ux + uy;
            constexpr size_t msb = sizeof(T) * 8 - 1;

            if constexpr (std::is_unsigned_v<T>)
            {
                sat |= res < x;
                res |= -(res < x);
            }
            else
            {
                ux = (ux >> msb) + ((UT{0x1} << msb) - 1); // saturate value
                if (static_cast<T>((ux ^ uy) | ~(uy ^ res)) >= 0)
                {
                    res = ux;
                    sat = true;
                }
            }

            return static_cast<T>(res);
        }
    };

    template <typename T> struct SatSub
    {
        inline T operator()(const T & x, const T & y) const
        {
            using UT = std::make_unsigned_t<T>;
            UT ux = x;
            UT uy = y;
            UT res = ux - uy;
            constexpr size_t msb = sizeof(T) * 8 - 1;

            if constexpr (std::is_unsigned_v<T>)
            {
                sat |= res > x;
                res &= -(res <= x);
            }
            else
            {
                ux = (ux >> msb) + ((UT{0x1} << msb) - 1); // saturate value
                if (static_cast<T>((ux ^ uy) & (ux ^ res)) < 0)
                {
                    res = ux;
                    sat = true;
                }
            }

            return static_cast<T>(res);
        }
    };

    template <typename T> struct AveAdd
    {
        inline T operator()(const T & x, const T & y) const
        {
            using UT = std::make_unsigned_t<T>;
            T res = x + y;
            bool c = (x < 0) ^ (y < 0) ^ (static_cast<UT>(res) < static_cast<UT>(x));
            res = intRounding(res, c, 1);
            return static_cast<T>((static_cast<UT>(res) >> 1) | (UT{c} << (sizeof(T) * 8 - 1)));
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
            return static_cast<T>((static_cast<UT>(res) >> 1) | (UT{c} << (sizeof(T) * 8 - 1)));
        }
    };

    template <typename T> struct SatMul
    {
        inline T operator()(const T & x, const T & y) const
        {
            static_assert(std::is_signed_v<T>, "Saturating Mul only supports signed integer.");
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

            return static_cast<T>((static_cast<UT>(res_l) >> (sizeof(T) * 8 - 1)) | (res_h << 1));
        }
    };

    template <typename T> struct SclSrl
    {
        inline T operator()(const T & x, const T & y) const
        {
            using UT = std::make_unsigned_t<T>;
            const size_t sh = y & (sizeof(T) * 8 - 1);
            bool c = false;
            T res = intRounding(x, c, sh);
            return static_cast<T>((static_cast<UT>(res) >> sh) | (UT{c} << (sizeof(T) * 8 - sh)));
        }
    };

    template <typename T> struct SclSra
    {
        inline T operator()(const T & x, const T & y) const
        {
            using UT = std::make_unsigned_t<T>;
            const size_t sh = y & (sizeof(T) * 8 - 1);
            bool c = false;
            T res = intRounding(x, c, sh);
            return static_cast<T>((static_cast<UT>(res) >> sh) | (-T{c} << (sizeof(T) * 8 - sh)));
        }
    };

    template <typename T> struct NClip
    {
        inline T operator()(const T & x, const T & y) const
        {
            using UT = std::make_unsigned_t<T>;
            const size_t sh = y & (sizeof(T) * 8 - 1);
            bool c = false;
            T res = intRounding(x, c, sh);
            if constexpr (std::is_unsigned_v<T>)
            {
                res = (res >> sh) | (T{c} << (sizeof(T) * 8 - sh));
            }
            else
            {
                res =
                    static_cast<T>((static_cast<UT>(res) >> sh) | (-T{c} << (sizeof(T) * 8 - sh)));
            }

            // clip to narrower destination
            if constexpr (std::is_unsigned_v<T>)
            {
                constexpr T nmax = (T{0x1} << ((sizeof(T) >> 1) * 8)) - 1;
                if (res > nmax)
                {
                    sat = true;
                    res = nmax;
                }
            }
            else
            {
                constexpr T nmax = (T{0x1} << ((sizeof(T) >> 1) * 8 - 1)) - 1;
                constexpr T nmin = T{0x1} << ((sizeof(T) >> 1) * 8);
                if (res < nmin)
                {
                    sat = true;
                    res = nmin;
                }
                else if (res > nmax)
                {
                    sat = true;
                    res = nmax;
                }
            }
            return res;
        }
    };
} // namespace pegasus
