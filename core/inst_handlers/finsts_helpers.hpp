#pragma once

#include <utility>
#include <tuple>

#include "core/AtlasState.hpp"

extern "C"
{
#include "source/RISCV/specialize.h"
}

namespace atlas
{
    template <typename XLEN> void updateFloatCsrs(AtlasState* state)
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

    template <typename T> struct FuncTraits;

    template <typename R, typename... Args> struct FuncTraits<R (*)(Args...)>
    {
        using ReturnType = R;
        using ArgsTuple = std::tuple<Args...>;
    };

    template <typename U> inline constexpr U usgnmask()
    {
        return static_cast<U>(1ULL << ((8 * sizeof(U)) - 1));
    }

    template <typename F> inline F fnegate(F f)
    {
        using U = decltype(f.v);
        return F{static_cast<U>(f.v ^ usgnmask<U>())};
    }

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

    // sign injection

    template <typename F> inline F fsgnj(F f1, F f2)
    {
        using U = decltype(f1.v);
        const U mask = usgnmask<U>();
        return F{static_cast<U>((f1.v & ~mask) | (f2.v & mask))};
    }

    template <typename F> inline F fsgnjn(F f1, F f2)
    {
        using U = decltype(f1.v);
        const U mask = usgnmask<U>();
        return F{static_cast<U>((f1.v & ~mask) | ((f2.v & mask) ^ mask))};
    }

    template <typename F> inline F fsgnjx(F f1, F f2)
    {
        using U = decltype(f1.v);
        const U mask = usgnmask<U>();
        return F{static_cast<U>((f1.v & ~mask) | ((f1.v ^ f2.v) & mask))};
    }

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

    /**
     * @brief Call *func* by adjusting arguments from uint_t and return uint_t as return value.
     *        Functor can be used instead of lambda.
     * @param func Wrapped function.
     * @param args Variadic function arguments of same type.
     */
    auto func_wrapper = [](auto func, auto... args)
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
    };
} // namespace atlas