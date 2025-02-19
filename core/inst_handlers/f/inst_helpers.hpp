#pragma once

#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"
#include "include/CSRBitMasks64.hpp"
#include "mavis/OpcodeInfo.h"

extern "C"
{
#include "source/include/softfloat.h"
}

namespace atlas
{
    using SP = uint32_t;
    using DP = uint64_t;

    template <typename XLEN> inline uint_fast8_t getRM(const AtlasInstPtr & inst, AtlasState* state)
    {
        uint64_t static_rm = inst->getRM();
        if (static_rm == 7) // RM field "DYN"
        {
            static_rm = READ_CSR_REG<XLEN>(state, FRM);
        }
        return static_rm;
    }

    template <typename XLEN> inline void update_csr(AtlasState* state)
    {
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

    template <typename T> ActionGroup* compute_address_handler(AtlasState* state)
    {
        static_assert(std::is_same<T, RV64>::value || std::is_same<T, RV32>::value);

        const AtlasInstPtr & inst = state->getCurrentInst();
        const T rs1_val = inst->getRs1Reg()->dmiRead<uint64_t>();
        constexpr uint32_t IMM_SIZE = 12;
        const T imm = inst->hasImmediate() ? inst->getSignExtendedImmediate<T, IMM_SIZE>() : 0;
        const T vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(T));
        return nullptr;
    }

    template <typename RV, typename SIZE, bool LOAD>
    ActionGroup* float_ls_handler(atlas::AtlasState* state)
    {
        static_assert(std::is_same<RV, RV64>::value || std::is_same<RV, RV32>::value);
        static_assert(std::is_same<SIZE, SP>::value || std::is_same<SIZE, DP>::value);
        static_assert(sizeof(RV) >= sizeof(SIZE));

        const AtlasInstPtr & inst = state->getCurrentInst();
        const RV paddr = state->getTranslationState()->getTranslationResult().getPaddr();

        if constexpr (LOAD)
        {
            inst->getRdReg()->dmiWrite(state->readMemory<SIZE>(paddr));
        }
        else
        {
            state->writeMemory<SIZE>(paddr, inst->getRs2Reg()->dmiRead<uint64_t>());
        }
        return nullptr;
    }
} // namespace atlas