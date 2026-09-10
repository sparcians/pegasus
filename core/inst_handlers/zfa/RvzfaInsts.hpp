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
