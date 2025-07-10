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

    template <typename F> F fnegate(F f)
    {
        using U = decltype(std::declval<F>().v);
        static const U sign_mask{static_cast<U>(1ULL << ((8 * sizeof(U)) - 1))};
        return F{static_cast<U>(f.v ^ sign_mask)};
    }

    template <typename F> F fmin(F f1, F f2)
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

    template <typename F> F fmax(F f1, F f2)
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
} // namespace atlas