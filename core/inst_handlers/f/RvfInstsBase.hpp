#pragma once

#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "include/PegasusUtils.hpp"
#include "include/gen/CSRBitMasks64.hpp"
#include "mavis/OpcodeInfo.h"
#include "core/inst_handlers/finst_helpers.hpp"

extern "C"
{
#include "source/RISCV/specialize.h"
#include "source/include/internals.h"
}

namespace pegasus
{
    class RvfInstsBase
    {
      public:
        using base_type = RvfInstsBase;

      protected:
        template <typename XLEN> inline uint_fast8_t getRM(PegasusState* state)
        {
            auto inst = state->getCurrentInst();
            uint64_t static_rm = inst->getRM();
            if (static_rm == 7) // RM field "DYN"
            {
                static_rm = READ_CSR_REG<XLEN>(state, FRM);
            }
            return static_rm;
        }

        // From RISCV manual:
        // the value −0.0 is considered to be less than the value +0.0. If both inputs are NaNs, the
        // result is the canonical NaN. If only one operand is a NaN, the result is the non-NaN
        // operand.
        template <typename SIZE>
        static void fmaxFminNanZeroCheck(SIZE rs1_val, SIZE rs2_val, SIZE & rd_val, bool max)
        {
            static_assert(std::is_same_v<SIZE, FLOAT_SP> || std::is_same_v<SIZE, FLOAT_DP>
                          || std::is_same_v<SIZE, FLOAT_HP>);

            const FConstants<SIZE> & cons = getConst<SIZE>();

            const bool rs1_nan =
                ((rs1_val & cons.EXP_MASK) == cons.EXP_MASK) && (rs1_val & cons.SIG_MASK);
            const bool rs2_nan =
                ((rs2_val & cons.EXP_MASK) == cons.EXP_MASK) && (rs2_val & cons.SIG_MASK);
            if (rs1_nan && rs2_nan)
            {
                rd_val = cons.CAN_NAN;
            }
            else if (rs1_nan)
            {
                rd_val = rs2_val;
            }
            else if (rs2_nan)
            {
                rd_val = rs1_val;
            }

            if (((rs1_val == cons.NEG_ZERO) && (rs2_val == cons.POS_ZERO))
                || ((rs2_val == cons.NEG_ZERO) && (rs1_val == cons.POS_ZERO)))
            {
                rd_val = max ? cons.POS_ZERO : cons.NEG_ZERO;
            }
        }

        template <typename SIZE>
        static void fmaxFminNanCheck(SIZE rs1_val, SIZE rs2_val, SIZE & rd_val, bool max)
        {
            static_assert(std::is_same<SIZE, FLOAT_SP>::value
                          || std::is_same<SIZE, FLOAT_DP>::value);

            const FConstants<SIZE> & cons = getConst<SIZE>();

            bool rs1_nan =
                ((rs1_val & cons.EXP_MASK) == cons.EXP_MASK) && (rs1_val & cons.SIG_MASK);
            bool rs2_nan =
                ((rs2_val & cons.EXP_MASK) == cons.EXP_MASK) && (rs2_val & cons.SIG_MASK);
            if (rs1_nan || rs2_nan)
            {
                rd_val = cons.CAN_NAN;
            }
            if (((rs1_val == cons.NEG_ZERO) && (rs2_val == cons.POS_ZERO))
                || ((rs2_val == cons.NEG_ZERO) && (rs1_val == cons.POS_ZERO)))
            {
                rd_val = max ? cons.POS_ZERO : cons.NEG_ZERO;
            }
        }

        template <typename XLEN> static void updateCsr(PegasusState* state)
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

        // For the trig functions, which do not raise fflags
        template <typename XLEN> static void updateNoFlagCsr(PegasusState* state)
        {
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

        template <typename XLEN>
        Action::ItrType computeAddressHandler(PegasusState* state, Action::ItrType action_it)
        {
            static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

            const PegasusInstPtr & inst = state->getCurrentInst();
            const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
            const XLEN imm = inst->getImmediate();
            const XLEN vaddr = rs1_val + imm;
            inst->getTranslationState()->makeRequest(vaddr, sizeof(XLEN));
            return ++action_it;
        }

        template <typename SIZE, bool LOAD>
        Action::ItrType floatLsHandler(PegasusState* state, Action::ItrType action_it)
        {
            static_assert(std::is_same_v<SIZE, FLOAT_SP> || std::is_same_v<SIZE, FLOAT_DP>
                          || std::is_same_v<SIZE, FLOAT_HP>);

            const PegasusInstPtr & inst = state->getCurrentInst();
            const Addr paddr = inst->getTranslationState()->getResult().getPAddr();
            inst->getTranslationState()->popResult();

            if constexpr (LOAD)
            {
                RV64 value = state->readMemory<SIZE>(paddr);
                value = nanBoxing<RV64, SIZE>(value);
                WRITE_FP_REG<RV64>(state, inst->getRd(), value);
            }
            else
            {
                state->writeMemory<SIZE>(paddr, READ_FP_REG<RV64>(state, inst->getRs2()));
            }
            return ++action_it;
        }
    }; // class RvfInstsBase
} // namespace pegasus
