#include "core/inst_handlers/zfa/RvzfaInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/PegasusUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "system/PegasusSystem.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzfaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // Single precision
        inst_handlers.emplace(
            "fleq.s",
            pegasus::Action::createAction<&RvzfaInsts::fleqHandler_<XLEN, FLOAT_SP>, RvzfaInsts>(
                nullptr, "fleq.s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fli.s",
            pegasus::Action::createAction<&RvzfaInsts::fliHandler_<XLEN, FLOAT_SP>, RvzfaInsts>(
                nullptr, "fli.s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fltq.s",
            pegasus::Action::createAction<&RvzfaInsts::fltqHandler_<XLEN, FLOAT_SP>, RvzfaInsts>(
                nullptr, "fltq.s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmaxm.s",
            pegasus::Action::createAction<&RvzfaInsts::fminmaxHandler_<XLEN, FLOAT_SP, true>,
                                          RvzfaInsts>(nullptr, "fmaxm.s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fminm.s",
            pegasus::Action::createAction<&RvzfaInsts::fminmaxHandler_<XLEN, FLOAT_SP, false>,
                                          RvzfaInsts>(nullptr, "fminm.s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fround.s",
            pegasus::Action::createAction<&RvzfaInsts::froundHandler_<XLEN, FLOAT_SP, false>,
                                          RvzfaInsts>(nullptr, "fround.s",
                                                      ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "froundnx.s",
            pegasus::Action::createAction<&RvzfaInsts::froundHandler_<XLEN, FLOAT_SP, true>,
                                          RvzfaInsts>(nullptr, "froundnx.s",
                                                      ActionTags::EXECUTE_TAG));

        // Double precision
        inst_handlers.emplace(
            "fleq.d",
            pegasus::Action::createAction<&RvzfaInsts::fleqHandler_<XLEN, FLOAT_DP>, RvzfaInsts>(
                nullptr, "fleq.d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fli.d",
            pegasus::Action::createAction<&RvzfaInsts::fliHandler_<XLEN, FLOAT_DP>, RvzfaInsts>(
                nullptr, "fli.d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fltq.d",
            pegasus::Action::createAction<&RvzfaInsts::fltqHandler_<XLEN, FLOAT_DP>, RvzfaInsts>(
                nullptr, "fltq.d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmaxm.d",
            pegasus::Action::createAction<&RvzfaInsts::fminmaxHandler_<XLEN, FLOAT_DP, true>,
                                          RvzfaInsts>(nullptr, "fmaxm.d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fminm.d",
            pegasus::Action::createAction<&RvzfaInsts::fminmaxHandler_<XLEN, FLOAT_DP, false>,
                                          RvzfaInsts>(nullptr, "fminm.d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fround.d",
            pegasus::Action::createAction<&RvzfaInsts::froundHandler_<XLEN, FLOAT_DP, false>,
                                          RvzfaInsts>(nullptr, "fround.d",
                                                      ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "froundnx.d",
            pegasus::Action::createAction<&RvzfaInsts::froundHandler_<XLEN, FLOAT_DP, true>,
                                          RvzfaInsts>(nullptr, "froundnx.d",
                                                      ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvtmod.w.d",
            pegasus::Action::createAction<&RvzfaInsts::fcvtmodHandler_<XLEN>, RvzfaInsts>(
                nullptr, "fcvtmod.w.d", ActionTags::EXECUTE_TAG));

        if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "fmvh.x.d",
                pegasus::Action::createAction<&RvzfaInsts::fmvh_x_dHandler_, RvzfaInsts>(
                    nullptr, "fmvh.x.d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmvp.d.x",
                pegasus::Action::createAction<&RvzfaInsts::fmvp_d_xHandler_, RvzfaInsts>(
                    nullptr, "fmvp.d.x", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzfaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzfaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, typename SIZE>
    Action::ItrType RvzfaInsts::fliHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = inst->getRs1();

        const SIZE rd_val = fli_table_.at(rs1_val);
        WRITE_FP_REG<FLOAT_DP>(state, inst->getRd(), nanBoxing<FLOAT_DP, SIZE>(rd_val));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN, typename SIZE, bool ISMAX>
    Action::ItrType RvzfaInsts::fminmaxHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const SIZE rs1_val =
            checkNanBoxing<RV64, SIZE>(READ_FP_REG<FLOAT_DP>(state, inst->getRs1()));
        const SIZE rs2_val =
            checkNanBoxing<RV64, SIZE>(READ_FP_REG<FLOAT_DP>(state, inst->getRs2()));
        SIZE rd_val;

        if constexpr (std::is_same_v<SIZE, FLOAT_SP>)
        {
            rd_val = f32_le_quiet(float32_t{rs1_val}, float32_t{rs2_val}) ? rs2_val : rs1_val;
            rd_val = nanBoxing<FLOAT_DP, SIZE>(rd_val);
        }
        else if constexpr (std::is_same_v<SIZE, FLOAT_DP>)
        {
            rd_val = f64_le_quiet(float64_t{rs1_val}, float64_t{rs2_val}) ? rs2_val : rs1_val;
        }
        else
        {
            static_assert(true, "unsupported floating point size");
        }

        fmaxFminNanCheck_<SIZE>(rs1_val, rs2_val, rd_val, ISMAX);
        WRITE_FP_REG<RV64>(state, inst->getRd(), rd_val);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN, typename SIZE, bool EXACT>
    Action::ItrType RvzfaInsts::froundHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const SIZE rs1_val =
            checkNanBoxing<RV64, SIZE>(READ_FP_REG<FLOAT_DP>(state, inst->getRs1()));
        softfloat_roundingMode = getRM<RV64>(state);

        if constexpr (std::is_same_v<SIZE, FLOAT_SP>)
        {
            WRITE_FP_REG<RV64>(
                state, inst->getRd(),
                nanBoxing<FLOAT_DP, SIZE>(
                    f32_roundToInt(float32_t{rs1_val}, softfloat_roundingMode, EXACT).v));
        }
        else if constexpr (std::is_same_v<SIZE, FLOAT_DP>)
        {
            WRITE_FP_REG<RV64>(state, inst->getRd(),
                               f64_roundToInt(float64_t{rs1_val}, softfloat_roundingMode, EXACT).v);
        }
        else
        {
            static_assert(true, "unsupported floating point size");
        }

        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfaInsts::fcvtmodHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_DP rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());

        // Always round towards zero, rm field is required to beset to 1
        softfloat_roundingMode = getRM<XLEN>(state);
        sparta_assert(softfloat_roundingMode == softfloat_round_minMag);

        // +/- infinity and NaN are converted to zero.
        XLEN result = 0;
        if (expF64UI(rs1_val) != 0x7FF)
        {
            result = signExtend<uint32_t, uint64_t>(
                f64_to_i32(float64_t{rs1_val}, getRM<RV64>(state), true));
        }
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    Action::ItrType RvzfaInsts::fmvh_x_dHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        // Move bits 63:32 of FP source to 32-bit INT dest
        const RV64 rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV32>(state, inst->getRd(), (rs1_val >> 32));
        return ++action_it;
    }

    Action::ItrType RvzfaInsts::fmvp_d_xHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        // Move 32-bit INT source to bits 31:0 of FP dest
        const uint64_t rs1_val = READ_INT_REG<RV32>(state, inst->getRs1());
        // Move 32-bit INT source to bits 63:32 of FP dest
        const uint64_t rs2_val = READ_INT_REG<RV32>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), ((rs2_val << 32) & rs1_val));
        return ++action_it;
    }

    template <typename XLEN, typename SIZE>
    Action::ItrType RvzfaInsts::fleqHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val =
            checkNanBoxing<RV64, SIZE>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val =
            checkNanBoxing<RV64, SIZE>(READ_FP_REG<RV64>(state, inst->getRs2()));

        if constexpr (std::is_same_v<SIZE, FLOAT_SP>)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                f32_le_quiet(float32_t{rs1_val}, float32_t{rs2_val}));
        }
        else if constexpr (std::is_same_v<SIZE, FLOAT_DP>)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                f64_le_quiet(float64_t{rs1_val}, float64_t{rs2_val}));
        }
        else
        {
            static_assert(true, "unsupported floating point size");
        }

        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN, typename SIZE>
    Action::ItrType RvzfaInsts::fltqHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val =
            checkNanBoxing<RV64, SIZE>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val =
            checkNanBoxing<RV64, SIZE>(READ_FP_REG<RV64>(state, inst->getRs2()));

        if constexpr (std::is_same_v<SIZE, FLOAT_SP>)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                f32_lt_quiet(float32_t{rs1_val}, float32_t{rs2_val}));
        }
        else if constexpr (std::is_same_v<SIZE, FLOAT_DP>)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                f64_lt_quiet(float64_t{rs1_val}, float64_t{rs2_val}));
        }
        else
        {
            static_assert(true, "unsupported floating point size");
        }

        updateCsr<XLEN>(state);
        return ++action_it;
    }
} // namespace pegasus
