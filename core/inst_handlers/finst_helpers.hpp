#pragma once

#include <utility>
#include <tuple>

#include "core/PegasusState.hpp"
#include "include/PegasusUtils.hpp"

extern "C"
{
#include "source/RISCV/specialize.h"
#include "source/include/internals.h"
}

namespace pegasus
{

    template <typename U> inline constexpr U usgnmask()
    {
        return static_cast<U>(1ULL << ((8 * sizeof(U)) - 1));
    }

    template <typename F> inline F fnegate(F f)
    {
        using U = decltype(f.v);
        return F{static_cast<U>(f.v ^ usgnmask<U>())};
    }

    template float16_t fnegate<float16_t>(float16_t);
    template float32_t fnegate<float32_t>(float32_t);
    template float64_t fnegate<float64_t>(float64_t);

    template <typename F> inline F fmin(F f1, F f2)
    {
        if constexpr (std::is_same_v<F, float16_t>)
        {
            return f16_le_quiet(f1, f2) ? f1 : f2;
        }
        else if constexpr (std::is_same_v<F, float32_t>)
        {
            return f32_le_quiet(f1, f2) ? f1 : f2;
        }
        else
        {
            return f64_le_quiet(f1, f2) ? f1 : f2;
        }
    }

    template float16_t fmin<float16_t>(float16_t, float16_t);
    template float32_t fmin<float32_t>(float32_t, float32_t);
    template float64_t fmin<float64_t>(float64_t, float64_t);

    template <typename F> inline F fmax(F f1, F f2)
    {
        if constexpr (std::is_same_v<F, float16_t>)
        {
            return f16_le_quiet(f1, f2) ? f2 : f1;
        }
        else if constexpr (std::is_same_v<F, float32_t>)
        {
            return f32_le_quiet(f1, f2) ? f2 : f1;
        }
        else
        {
            return f64_le_quiet(f1, f2) ? f2 : f1;
        }
    }

    template float16_t fmax<float16_t>(float16_t, float16_t);
    template float32_t fmax<float32_t>(float32_t, float32_t);
    template float64_t fmax<float64_t>(float64_t, float64_t);

    // sign injection

    template <typename F> inline F fsgnj(F f1, F f2)
    {
        using U = decltype(f1.v);
        const U mask = usgnmask<U>();
        return F{static_cast<U>((f1.v & ~mask) | (f2.v & mask))};
    }

    template float16_t fsgnj<float16_t>(float16_t, float16_t);
    template float32_t fsgnj<float32_t>(float32_t, float32_t);
    template float64_t fsgnj<float64_t>(float64_t, float64_t);

    template <typename F> inline F fsgnjn(F f1, F f2)
    {
        using U = decltype(f1.v);
        const U mask = usgnmask<U>();
        return F{static_cast<U>((f1.v & ~mask) | ((f2.v & mask) ^ mask))};
    }

    template float16_t fsgnjn<float16_t>(float16_t, float16_t);
    template float32_t fsgnjn<float32_t>(float32_t, float32_t);
    template float64_t fsgnjn<float64_t>(float64_t, float64_t);

    template <typename F> inline F fsgnjx(F f1, F f2)
    {
        using U = decltype(f1.v);
        const U mask = usgnmask<U>();
        return F{static_cast<U>((f1.v & ~mask) | ((f1.v ^ f2.v) & mask))};
    }

    template float16_t fsgnjx<float16_t>(float16_t, float16_t);
    template float32_t fsgnjx<float32_t>(float32_t, float32_t);
    template float64_t fsgnjx<float64_t>(float64_t, float64_t);

    // compare

    template <typename F> inline bool fne(F f1, F f2)
    {
        if constexpr (std::is_same_v<F, float16_t>)
        {
            return !f16_eq(f1, f2);
        }
        else if constexpr (std::is_same_v<F, float32_t>)
        {
            return !f32_eq(f1, f2);
        }
        else
        {
            return !f64_eq(f1, f2);
        }
    }

    template bool fne<float16_t>(float16_t, float16_t);
    template bool fne<float32_t>(float32_t, float32_t);
    template bool fne<float64_t>(float64_t, float64_t);

    template <typename F> inline bool fgt(F f1, F f2)
    {
        if constexpr (std::is_same_v<F, float16_t>)
        {
            return !f16_le(f1, f2);
        }
        else if constexpr (std::is_same_v<F, float32_t>)
        {
            return !f32_le(f1, f2);
        }
        else
        {
            return !f64_le(f1, f2);
        }
    }

    template bool fgt<float16_t>(float16_t, float16_t);
    template bool fgt<float32_t>(float32_t, float32_t);
    template bool fgt<float64_t>(float64_t, float64_t);

    template <typename F> inline bool fge(F f1, F f2)
    {
        if constexpr (std::is_same_v<F, float16_t>)
        {
            return !f16_lt(f1, f2);
        }
        else if constexpr (std::is_same_v<F, float32_t>)
        {
            return !f32_lt(f1, f2);
        }
        else
        {
            return !f64_lt(f1, f2);
        }
    }

    template bool fge<float16_t>(float16_t, float16_t);
    template bool fge<float32_t>(float32_t, float32_t);
    template bool fge<float64_t>(float64_t, float64_t);

    template <typename U> inline uint16_t fclass(U u)
    {
        const bool infOrNaN = std::is_same_v<U, uint16_t>   ? expF16UI(u) == 0x1F
                              : std::is_same_v<U, uint32_t> ? expF32UI(u) == 0xFF
                                                            : expF64UI(u) == 0x7FF;
        const bool subnormalOrZero = (std::is_same_v<U, uint16_t>   ? expF16UI(u)
                                      : std::is_same_v<U, uint32_t> ? expF32UI(u)
                                                                    : expF64UI(u))
                                     == 0;
        const bool sign = std::is_same_v<U, uint16_t>   ? signF16UI(u)
                          : std::is_same_v<U, uint32_t> ? signF32UI(u)
                                                        : signF64UI(u);
        const bool fracZero = (std::is_same_v<U, uint16_t>   ? fracF16UI(u)
                               : std::is_same_v<U, uint32_t> ? fracF32UI(u)
                                                             : fracF64UI(u))
                              == 0;
        const bool isNaN = std::is_same_v<U, uint16_t>   ? isNaNF16UI(u)
                           : std::is_same_v<U, uint32_t> ? isNaNF32UI(u)
                                                         : isNaNF64UI(u);
        const bool isSNaN = std::is_same_v<U, uint16_t>   ? softfloat_isSigNaNF16UI(u)
                            : std::is_same_v<U, uint32_t> ? softfloat_isSigNaNF32UI(u)
                                                          : softfloat_isSigNaNF64UI(u);

        uint16_t val = 0;

        // Negative infinity
        if (sign && infOrNaN && fracZero)
        {
            val |= 1 << 0;
        }

        // Negative normal number
        if (sign && !infOrNaN && !subnormalOrZero)
        {
            val |= 1 << 1;
        }

        // Negative subnormal number
        if (sign && subnormalOrZero && !fracZero)
        {
            val |= 1 << 2;
        }

        // Negative zero
        if (sign && subnormalOrZero && fracZero)
        {
            val |= 1 << 3;
        }

        // Positive infinity
        if (!sign && infOrNaN && fracZero)
        {
            val |= 1 << 7;
        }

        // Positive normal number
        if (!sign && !infOrNaN && !subnormalOrZero)
        {
            val |= 1 << 6;
        }

        // Positive subnormal number
        if (!sign && subnormalOrZero && !fracZero)
        {
            val |= 1 << 5;
        }

        // Positive zero
        if (!sign && subnormalOrZero && fracZero)
        {
            val |= 1 << 4;
        }

        // Signaling NaN
        if (isNaN && isSNaN)
        {
            val |= 1 << 8;
        }

        // Quiet NaN
        if (isNaN && !isSNaN)
        {
            val |= 1 << 9;
        }
        return val;
    }

    template uint16_t fclass<uint16_t>(uint16_t);
    template uint16_t fclass<uint32_t>(uint32_t);
    template uint16_t fclass<uint64_t>(uint64_t);

    static inline uint64_t extract64(uint64_t val, int pos, int len)
    {
        assert(pos >= 0 && len > 0 && len <= 64 - pos);
        return (val >> pos) & (~UINT64_C(0) >> (64 - len));
    }

    static inline uint64_t make_mask64(int pos, int len)
    {
        assert(pos >= 0 && len > 0 && pos < 64 && len <= 64);
        return (UINT64_MAX >> (64 - len)) << pos;
    }

    // user needs to truncate output to required length
    static inline uint64_t recip7(uint64_t val, int e, int s, int rm, bool sub,
                                  bool* round_abnormal)
    {
        uint64_t exp = extract64(val, s, e);
        uint64_t sig = extract64(val, 0, s);
        uint64_t sign = extract64(val, s + e, 1);
        const int p = 7;

        static const uint8_t table[] = {
            127, 125, 123, 121, 119, 117, 116, 114, 112, 110, 109, 107, 105, 104, 102, 100,
            99,  97,  96,  94,  93,  91,  90,  88,  87,  85,  84,  83,  81,  80,  79,  77,
            76,  75,  74,  72,  71,  70,  69,  68,  66,  65,  64,  63,  62,  61,  60,  59,
            58,  57,  56,  55,  54,  53,  52,  51,  50,  49,  48,  47,  46,  45,  44,  43,
            42,  41,  40,  40,  39,  38,  37,  36,  35,  35,  34,  33,  32,  31,  31,  30,
            29,  28,  28,  27,  26,  25,  25,  24,  23,  23,  22,  21,  21,  20,  19,  19,
            18,  17,  17,  16,  15,  15,  14,  14,  13,  12,  12,  11,  11,  10,  9,   9,
            8,   8,   7,   7,   6,   5,   5,   4,   4,   3,   3,   2,   2,   1,   1,   0};

        if (sub)
        {
            while (extract64(sig, s - 1, 1) == 0)
                exp--, sig <<= 1;

            sig = (sig << 1) & make_mask64(0, s);

            if (exp != 0 && exp != UINT64_MAX)
            {
                *round_abnormal = true;
                if (rm == softfloat_round_minMag || (rm == softfloat_round_min && !sign)
                    || (rm == softfloat_round_max && sign))
                    return ((sign << (s + e)) | make_mask64(s, e)) - 1; // max mag saturate
                else
                    return (sign << (s + e)) | make_mask64(s, e); // infinite
            }
        }

        int idx = sig >> (s - p);
        uint64_t out_sig = (uint64_t)(table[idx]) << (s - p);
        uint64_t out_exp = 2 * make_mask64(0, e - 1) + ~exp;
        if (out_exp == 0 || out_exp == UINT64_MAX)
        {
            out_sig = (out_sig >> 1) | make_mask64(s - 1, 1);
            if (out_exp == UINT64_MAX)
            { // submormal
                out_sig >>= 1;
                out_exp = 0;
            }
        }

        return (sign << (s + e)) | (out_exp << s) | out_sig;
    }

    template <typename U> inline U frecip7(U u)
    {
        U ret = 0;

        auto cls = fclass(u);
        bool sub = false;
        bool round_abnormal = false;
        switch (cls)
        {
            case 0x001: // -inf
                ret = static_cast<U>(std::is_same_v<U, uint16_t>   ? 0x8000
                                     : std::is_same_v<U, uint32_t> ? 0x80000000
                                                                   : 0x8000000000000000);
                break;
            case 0x080: //+inf
                ret = static_cast<U>(std::is_same_v<U, uint16_t>   ? 0x0000
                                     : std::is_same_v<U, uint32_t> ? 0x00000000
                                                                   : 0x0000000000000000);
                break;
            case 0x008: // -0
                ret = static_cast<U>(std::is_same_v<U, uint16_t>   ? 0xfc00
                                     : std::is_same_v<U, uint32_t> ? 0xff800000
                                                                   : 0xfff0000000000000);
                softfloat_exceptionFlags |= softfloat_flag_infinite;
                break;
            case 0x010: // +0
                ret = static_cast<U>(std::is_same_v<U, uint16_t>   ? 0x7c00
                                     : std::is_same_v<U, uint32_t> ? 0x7f800000
                                                                   : 0x7ff0000000000000);
                softfloat_exceptionFlags |= softfloat_flag_infinite;
                break;
            case 0x100: // sNaN
                softfloat_exceptionFlags |= softfloat_flag_invalid;
                [[fallthrough]];
            case 0x200: // qNaN
                ret = static_cast<U>(std::is_same_v<U, uint16_t>   ? defaultNaNF16UI
                                     : std::is_same_v<U, uint32_t> ? defaultNaNF32UI
                                                                   : defaultNaNF64UI);
                break;
            case 0x004: // -subnormal
                [[fallthrough]];
            case 0x020: //+ sub
                sub = true;
                [[fallthrough]];
            default: // +- normal
                ret = static_cast<U>(
                    std::is_same_v<U, uint16_t>
                        ? recip7(ret, 5, 10, softfloat_roundingMode, sub, &round_abnormal)
                    : std::is_same_v<U, uint32_t>
                        ? recip7(ret, 8, 23, softfloat_roundingMode, sub, &round_abnormal)
                        : recip7(ret, 11, 52, softfloat_roundingMode, sub, &round_abnormal));
                if (round_abnormal)
                    softfloat_exceptionFlags |= softfloat_flag_inexact | softfloat_flag_overflow;
                break;
        }

        return ret;
    }

    template uint16_t frecip7<uint16_t>(uint16_t);
    template uint32_t frecip7<uint32_t>(uint32_t);
    template uint64_t frecip7<uint64_t>(uint64_t);

    // user needs to truncate output to required length
    static inline uint64_t rsqrt7(uint64_t val, int e, int s, bool sub)
    {
        uint64_t exp = extract64(val, s, e);
        uint64_t sig = extract64(val, 0, s);
        uint64_t sign = extract64(val, s + e, 1);
        const int p = 7;

        static const uint8_t table[] = {
            52,  51,  50,  48,  47,  46,  44,  43,  42,  41,  40,  39,  38,  36,  35,  34,
            33,  32,  31,  30,  30,  29,  28,  27,  26,  25,  24,  23,  23,  22,  21,  20,
            19,  19,  18,  17,  16,  16,  15,  14,  14,  13,  12,  12,  11,  10,  10,  9,
            9,   8,   7,   7,   6,   6,   5,   4,   4,   3,   3,   2,   2,   1,   1,   0,
            127, 125, 123, 121, 119, 118, 116, 114, 113, 111, 109, 108, 106, 105, 103, 102,
            100, 99,  97,  96,  95,  93,  92,  91,  90,  88,  87,  86,  85,  84,  83,  82,
            80,  79,  78,  77,  76,  75,  74,  73,  72,  71,  70,  70,  69,  68,  67,  66,
            65,  64,  63,  63,  62,  61,  60,  59,  59,  58,  57,  56,  56,  55,  54,  53};

        if (sub)
        {
            while (extract64(sig, s - 1, 1) == 0)
                exp--, sig <<= 1;

            sig = (sig << 1) & make_mask64(0, s);
        }

        // even exp: sig >> 1, odd exp: sig >> 2
        int idx = ((exp & 1) << (p - 1)) | (sig >> (s - p + 1));
        uint64_t out_sig = (uint64_t)(table[idx]) << (s - p);
        uint64_t out_exp = (3 * make_mask64(0, e - 1) + ~exp) / 2;

        return (sign << (s + e)) | (out_exp << s) | out_sig;
    }

    template <typename U> inline U frsqrt7(U u)
    {
        U ret = 0;

        auto cls = fclass(u);
        bool sub = false;
        switch (cls)
        {
            case 0x001: // -inf
            case 0x002: // -normal
            case 0x004: // -subnormal
            case 0x100: // sNaN
                softfloat_exceptionFlags |= softfloat_flag_invalid;
                [[fallthrough]];
            case 0x200: // qNaN
                ret = static_cast<U>(std::is_same_v<U, uint16_t>   ? defaultNaNF16UI
                                     : std::is_same_v<U, uint32_t> ? defaultNaNF32UI
                                                                   : defaultNaNF64UI);
                break;
            case 0x008: // -0
                ret = static_cast<U>(std::is_same_v<U, uint16_t>   ? 0xfc00
                                     : std::is_same_v<U, uint32_t> ? 0xff800000
                                                                   : 0xfff0000000000000);
                softfloat_exceptionFlags |= softfloat_flag_infinite;
                break;
            case 0x010: // +0
                ret = static_cast<U>(std::is_same_v<U, uint16_t>   ? 0x7c00
                                     : std::is_same_v<U, uint32_t> ? 0x7f800000
                                                                   : 0x7ff0000000000000);
                softfloat_exceptionFlags |= softfloat_flag_infinite;
                break;
            case 0x080: //+inf
                ret = 0x0;
                break;
            case 0x020: //+ sub
                sub = true;
                [[fallthrough]];
            default: // +num
                ret = static_cast<U>(std::is_same_v<U, uint16_t>   ? rsqrt7(ret, 5, 10, sub)
                                     : std::is_same_v<U, uint32_t> ? rsqrt7(ret, 8, 23, sub)
                                                                   : rsqrt7(ret, 11, 52, sub));
                break;
        }

        return ret;
    }

    template uint16_t frsqrt7<uint16_t>(uint16_t);
    template uint32_t frsqrt7<uint32_t>(uint32_t);
    template uint64_t frsqrt7<uint64_t>(uint64_t);

    // helper functions

    template <typename XLEN> void restoreFloatCsrs(PegasusState* state)
    {
        decltype(softfloat_exceptionFlags) value = 0;
        auto csr = READ_CSR_REG<XLEN>(state, FFLAGS);

        auto syncBitField =
            [&csr, &value](uint32_t csr_num, const char* field_name, exceptionFlag_t flag)
        {
            const auto & csr_bit_range = getCsrBitRange<XLEN>(csr_num, field_name);
            const auto lsb = csr_bit_range.first;
            const auto msb = csr_bit_range.second;
            const XLEN mask = ((XLEN{1} << (msb - lsb + 1)) - 1) << lsb;
            value |= ((csr & mask) != 0) * flag;
        };

        syncBitField(FFLAGS, "NX", softfloat_flag_inexact);
        syncBitField(FFLAGS, "UF", softfloat_flag_underflow);
        syncBitField(FFLAGS, "OF", softfloat_flag_overflow);
        syncBitField(FFLAGS, "DZ", softfloat_flag_infinite);
        syncBitField(FFLAGS, "NV", softfloat_flag_invalid);
        softfloat_exceptionFlags = value;
    }

    template <typename XLEN> void saveFloatCsrs(PegasusState* state)
    {
        XLEN mask = 0;
        XLEN value = 0;

        auto updateBitField =
            [&mask, &value](uint32_t csr_num, const char* field_name, exceptionFlag_t flag)
        {
            const auto & csr_bit_range = getCsrBitRange<XLEN>(csr_num, field_name);
            const auto lsb = csr_bit_range.first;
            const auto msb = csr_bit_range.second;
            value |= static_cast<XLEN>((softfloat_exceptionFlags & flag) != 0) << lsb;
            mask |= ((XLEN{1} << (msb - lsb + 1)) - 1) << lsb;
        };

        // FFLAGS
        auto fflags = READ_CSR_REG<XLEN>(state, FFLAGS);
        updateBitField(FFLAGS, "NX", softfloat_flag_inexact);
        updateBitField(FFLAGS, "UF", softfloat_flag_underflow);
        updateBitField(FFLAGS, "OF", softfloat_flag_overflow);
        updateBitField(FFLAGS, "DZ", softfloat_flag_infinite);
        updateBitField(FFLAGS, "NV", softfloat_flag_invalid);
        WRITE_CSR_REG<XLEN>(state, FFLAGS, (fflags & ~mask) | value);

        mask = 0;
        value = 0;
        // FCSR
        auto fcsr = READ_CSR_REG<XLEN>(state, FCSR);
        updateBitField(FCSR, "NX", softfloat_flag_inexact);
        updateBitField(FCSR, "UF", softfloat_flag_underflow);
        updateBitField(FCSR, "OF", softfloat_flag_overflow);
        updateBitField(FCSR, "DZ", softfloat_flag_infinite);
        updateBitField(FCSR, "NV", softfloat_flag_invalid);
        WRITE_CSR_REG<XLEN>(state, FCSR, (fcsr & ~mask) | value);
    }

    /**
     * @brief Call *func* by adjusting arguments from uint_t and return uint_t as return value.
     *        Functor can be used instead of lambda.
     * @param func Wrapped function.
     * @param args Variadic function arguments of same type.
     */
    inline auto func_wrapper = [](auto func, auto... args)
    {
        using Traits = FuncTraits<decltype(func)>;
        using ReturnType = typename Traits::ReturnType;
        using ArgType = std::tuple_element_t<0, typename Traits::ArgsTuple>;
        ReturnType r;

        if constexpr (std::is_integral_v<ArgType>)
        {
            r = std::apply(func, std::make_tuple((static_cast<ArgType>(args), ...)));
        }
        else
        {
            using IntT = decltype(std::declval<ArgType>().v);
            r = std::apply(func, std::make_tuple((ArgType{static_cast<IntT>(args)}, ...)));
        }

        if constexpr (std::is_integral_v<ReturnType>)
        {
            return r;
        }
        else // floatX type
        {
            return r.v;
        }
    }; // func_wrapper

    // SP, DP constants and Nan-Boxing related

    template <typename SIZE> struct FConstants
    {
        static_assert(std::is_same_v<SIZE, FLOAT_SP> || std::is_same_v<SIZE, FLOAT_DP>
                      || std::is_same_v<SIZE, FLOAT_HP>);

        static constexpr uint8_t SGN_BIT = sizeof(SIZE) * 8 - 1;
        static constexpr uint8_t EXP_MSB = SGN_BIT - 1;
        static constexpr uint8_t EXP_LSB = std::is_same_v<SIZE, FLOAT_DP>   ? 52
                                           : std::is_same_v<SIZE, FLOAT_SP> ? 23
                                                                            : 10;
        static constexpr uint8_t SIG_MSB = EXP_LSB - 1;

        static constexpr SIZE EXP_MASK = (((SIZE)1 << (EXP_MSB - EXP_LSB + 1)) - 1) << EXP_LSB;
        static constexpr SIZE SIG_MASK = ((SIZE)1 << (SIG_MSB + 1)) - 1;

        static constexpr SIZE CAN_NAN = EXP_MASK | (SIZE)1 << SIG_MSB;
        static constexpr SIZE NEG_ZERO = (SIZE)1 << SGN_BIT;
        static constexpr SIZE POS_ZERO = 0;
    }; // struct FConstants

    constexpr FConstants<FLOAT_SP> const_sp{};
    constexpr FConstants<FLOAT_DP> const_dp{};
    constexpr FConstants<FLOAT_HP> const_hp{};

    template <typename SIZE> static constexpr FConstants<SIZE> getConst()
    {
        static_assert(std::is_same_v<SIZE, FLOAT_SP> || std::is_same_v<SIZE, FLOAT_DP>
                      || std::is_same_v<SIZE, FLOAT_HP>);
        if constexpr (std::is_same_v<SIZE, FLOAT_DP>)
        {
            return const_dp;
        }
        else if constexpr (std::is_same_v<SIZE, FLOAT_SP>)
        {
            return const_sp;
        }
        else
        {
            return const_hp;
        }
    }

    // Check and convert to a narrower SIZE floating point value from wider floating point
    // register.
    template <typename XLEN, typename SIZE> SIZE checkNanBoxing(XLEN num)
    {
        if constexpr (sizeof(XLEN) > sizeof(SIZE))
        {
            const FConstants<SIZE> & cons = getConst<SIZE>();
            constexpr XLEN mask = SIZE(-1);
            SIZE value = cons.CAN_NAN;
            if ((num & ~mask) == ~mask) // upper bits all 1's
            {
                value = num; // truncated
            }
            return value;
        }
        return num;
    }

    // NaN-boxing a narrower SIZE floating point value for wider floating point register.
    template <typename XLEN, typename SIZE> inline XLEN nanBoxing(XLEN num)
    {
        if constexpr (sizeof(XLEN) > sizeof(SIZE))
        {
            constexpr XLEN mask = SIZE(-1);
            return ~mask | (num & mask);
        }
        return num;
    }
} // namespace pegasus
