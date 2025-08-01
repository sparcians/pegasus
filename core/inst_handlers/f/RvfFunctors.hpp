#pragma once

#include "include/PegasusUtils.hpp"
#include "core/inst_handlers/finsts_helpers.hpp"

#include <stdint.h>

extern "C"
{
#include "source/RISCV/specialize.h"
#include "source/include/internals.h"
}

namespace pegasus
{
    template <typename F> constexpr auto getMulAdd()
    {
        if constexpr (std::is_same_v<F, float16_t>)
        {
            return f16_mulAdd;
        }
        else if constexpr (std::is_same_v<F, float32_t>)
        {
            return f32_mulAdd;
        }
        else if constexpr (std::is_same_v<F, float64_t>)
        {
            return f64_mulAdd;
        }
        else
        {
            return nullptr;
        }
    }

    template <typename F> struct Fmadd
    {
        F operator()(const F & a, const F & b, const F & c) const
        {
            constexpr auto mulAdd = getMulAdd<F>();
            return mulAdd(a, b, c);
        }
    };

    template <typename F> struct Fmsub
    {
        F operator()(const F & a, const F & b, const F & c) const
        {
            constexpr auto mulAdd = getMulAdd<F>();
            return mulAdd(a, b, fnegate(c));
        }
    };

    template <typename F> struct Fnmadd
    {
        F operator()(const F & a, const F & b, const F & c) const
        {
            constexpr auto mulAdd = getMulAdd<F>();
            return mulAdd(a, fnegate(b), fnegate(c));
        }
    };

    template <typename F> struct Fnmsub
    {
        F operator()(const F & a, const F & b, const F & c) const
        {
            constexpr auto mulAdd = getMulAdd<F>();
            return mulAdd(a, fnegate(b), c);
        }
    };

    template <typename F> struct Fmacc
    {
        F operator()(const F & a, const F & b, const F & c) const
        {
            constexpr auto mulAdd = getMulAdd<F>();
            return mulAdd(a, c, b);
        }
    };

    template <typename F> struct Fnmacc
    {
        F operator()(const F & a, const F & b, const F & c) const
        {
            constexpr auto mulAdd = getMulAdd<F>();
            return mulAdd(a, fnegate(c), fnegate(b));
        }
    };

    template <typename F> struct Fmsac
    {
        F operator()(const F & a, const F & b, const F & c) const
        {
            constexpr auto mulAdd = getMulAdd<F>();
            return mulAdd(a, c, fnegate(b));
        }
    };

    template <typename F> struct Fnmsac
    {
        F operator()(const F & a, const F & b, const F & c) const
        {
            constexpr auto mulAdd = getMulAdd<F>();
            return mulAdd(a, fnegate(c), b);
        }
    };
} // namespace pegasus