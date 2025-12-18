#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"
#include "core/inst_handlers/f/RvfInstsBase.hpp"

#include <map>
#include <string>
#include <math.h>

namespace pegasus
{
    class PegasusState;

    class RvzfaInsts : public RvfInstsBase
    {
      public:
        using base_type = RvzfaInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        // fli
        template <typename XLEN, typename FMT>
        Action::ItrType fliHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        inline static const std::array<uint32_t, 32> fli_table_ = {
            0xBF800000, // -1.0
            0x00800000, // Minimum positive normal
            0x37800000, // 1.0 * 2^-16
            0x38000000, // 1.0 * 2^-15
            0x3B800000, // 1.0 * 2^-8
            0x3C000000, // 1.0 * 2^-7
            0x3D800000, // 0.0625 (2^-4)
            0x3E000000, // 0.125 (2^-3)
            0x3E800000, // 0.25
            0x3EA00000, // 0.3125
            0x3EC00000, // 0.375
            0x3EE00000, // 0.4375
            0x3F000000, // 0.5
            0x3F200000, // 0.625
            0x3F400000, // 0.75
            0x3F600000, // 0.875
            0X3F800000, // 1.0
            0X3FA00000, // 1.25
            0X3FC00000, // 1.5
            0X3FE00000, // 1.75
            0X40000000, // 2.0
            0X40200000, // 2.5
            0X40400000, // 3
            0X40800000, // 4
            0X41000000, // 8
            0X41800000, // 16
            0X43000000, // 128 (2^7)
            0X43800000, // 256 (2^8)
            0X47000000, // 2^15
            0X47800000, // 2^16
            0X7F800000, // +infinity
            0x7FC00000  // Canonical NaN
        };

        // fminm, fmaxm
        template <typename XLEN, typename SIZE, bool ISMAX>
        Action::ItrType fminmaxHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // fround, froundnx
        template <typename XLEN, typename SIZE, bool EXACT>
        Action::ItrType froundHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // fcvtmod
        template <typename XLEN>
        Action::ItrType fcvtmodHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // fmvh.x.d, fmvp.d.x (RV32 only, DP only)
        Action::ItrType fmvh_x_dHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType fmvp_d_xHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // fleq, fltq
        template <typename XLEN, typename FMT>
        Action::ItrType fleqHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename FMT>
        Action::ItrType fltqHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename SIZE>
        static void fmaxFminNanCheck_(SIZE rs1_val, SIZE rs2_val, SIZE & rd_val, bool max)
        {
            static_assert(std::is_same<SIZE, FLOAT_SP>::value || std::is_same<SIZE, FLOAT_DP>::value
                          || std::is_same<SIZE, FLOAT_HP>::value);

            const FConstants<SIZE> & cons = getConst<SIZE>();

            const bool rs1_nan =
                ((rs1_val & cons.EXP_MASK) == cons.EXP_MASK) && (rs1_val & cons.SIG_MASK);
            const bool rs2_nan =
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
    };
} // namespace pegasus
