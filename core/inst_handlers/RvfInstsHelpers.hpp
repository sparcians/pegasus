#pragma once

#include "core/AtlasState.hpp"

extern "C"
{
#include "source/RISCV/specialize.h"
}

namespace atlas
{
    template <typename XLEN> void floatUpdateCsr(AtlasState* state)
    {
        // TODO: it would be nice to have field shift, then a single combined CSR write will
        // suffice.

        // FFLAGS
        WRITE_CSR_FIELD<XLEN>(
            state, FFLAGS, "NX",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_inexact) != 0));
        WRITE_CSR_FIELD<XLEN>(
            state, FFLAGS, "UF",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_underflow) != 0));
        WRITE_CSR_FIELD<XLEN>(
            state, FFLAGS, "OF",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_overflow) != 0));
        WRITE_CSR_FIELD<XLEN>(
            state, FFLAGS, "DZ",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_infinite) != 0));
        WRITE_CSR_FIELD<XLEN>(
            state, FFLAGS, "NV",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_invalid) != 0));

        // FCSR
        WRITE_CSR_FIELD<XLEN>(
            state, FCSR, "NX",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_inexact) != 0));
        WRITE_CSR_FIELD<XLEN>(
            state, FCSR, "UF",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_underflow) != 0));
        WRITE_CSR_FIELD<XLEN>(
            state, FCSR, "OF",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_overflow) != 0));
        WRITE_CSR_FIELD<XLEN>(
            state, FCSR, "DZ",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_infinite) != 0));
        WRITE_CSR_FIELD<XLEN>(
            state, FCSR, "NV",
            static_cast<uint64_t>((softfloat_exceptionFlags & softfloat_flag_invalid) != 0));
    }
} // namespace atlas