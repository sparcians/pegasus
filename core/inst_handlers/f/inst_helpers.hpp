#pragma once

#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"
#include "include/CSRBitMasks64.hpp"
#include "mavis/OpcodeInfo.h"
#include "arch/register_macros.hpp"

extern "C"
{
#include "source/include/softfloat.h"
}


namespace atlas
{
    constexpr uint64_t rv64_l32_mask = 0x0000FFFF;
    
    inline uint_fast8_t getRM(const AtlasInstPtr & inst, AtlasState* state)
    {
        uint64_t static_rm = inst->getRM();
        if (static_rm == 7) // RM field "DYN"
        {
            static_rm = READ_CSR_REG(FRM);
        }
        return static_rm;
    }
    
    // TODO: use official WRITE_CSR_FIELD when it's fixed.
    #ifdef WRITE_CSR_FIELD
    #undef WRITE_CSR_FIELD
    #define WRITE_CSR_FIELD(reg_name, field_name, field_value)                                     \
    {                                                                                              \
        const auto old_value =                                                                     \
            state->getCsrRegister(atlas::CSR::reg_name::reg_num)->dmiRead<uint64_t>();             \
        WRITE_CSR_REG(reg_name,                                                                    \
                      (old_value & ~reg_name ## _64_bitmasks::field_name) |                        \
                      (field_value & reg_name ## _64_bitmasks::field_name));                       \
    }
    #endif
    
    inline void update_csr(AtlasState* state)
    {
        // FFLAGS
        WRITE_CSR_FIELD(FFLAGS, NX, softfloat_exceptionFlags & softfloat_flag_inexact);
        WRITE_CSR_FIELD(FFLAGS, UF, softfloat_exceptionFlags & softfloat_flag_underflow);
        WRITE_CSR_FIELD(FFLAGS, OF, softfloat_exceptionFlags & softfloat_flag_overflow);
        WRITE_CSR_FIELD(FFLAGS, DZ, softfloat_exceptionFlags & softfloat_flag_infinite);
        WRITE_CSR_FIELD(FFLAGS, NV, softfloat_exceptionFlags & softfloat_flag_invalid);

        // FCSR
        WRITE_CSR_FIELD(FCSR, NX, softfloat_exceptionFlags & softfloat_flag_inexact);
        WRITE_CSR_FIELD(FCSR, UF, softfloat_exceptionFlags & softfloat_flag_underflow);
        WRITE_CSR_FIELD(FCSR, OF, softfloat_exceptionFlags & softfloat_flag_overflow);
        WRITE_CSR_FIELD(FCSR, DZ, softfloat_exceptionFlags & softfloat_flag_infinite);
        WRITE_CSR_FIELD(FCSR, NV, softfloat_exceptionFlags & softfloat_flag_invalid);

        // clear softfloat flag variable
        softfloat_exceptionFlags = 0;
    }

    template <typename T> ActionGroup* compute_address_handler(AtlasState* state)
    {
        static_assert(std::is_same<T, RV64>::value || std::is_same<T, RV32>::value);

        const AtlasInstPtr & inst = state->getCurrentInst();
        const T rs1_val = inst->getRs1()->dmiRead<uint64_t>();
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
            inst->getRd()->write(state->readMemory<SIZE>(paddr));
        }
        else
        {
            state->writeMemory<SIZE>(paddr, inst->getRs2()->dmiRead<uint64_t>());
        }
        return nullptr;
    }
} // namespace atlas