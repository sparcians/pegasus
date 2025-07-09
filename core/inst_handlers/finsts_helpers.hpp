#pragma once

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

    float16_t negate(float16_t f)
    {
        constexpr uint16_t sign_mask = 0x8000;
        return float16_t{static_cast<uint16_t>(f.v ^ sign_mask)};
    }

    float32_t negate(float32_t f)
    {
        constexpr uint32_t sign_mask = 0x80000000;
        return float32_t{static_cast<uint32_t>(f.v ^ sign_mask)};
    }

    float64_t negate(float64_t f)
    {
        constexpr uint64_t sign_mask = 0x8000000000000000;
        return float64_t{static_cast<uint64_t>(f.v ^ sign_mask)};
    }
} // namespace atlas