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
        {
            inst_handlers.emplace(
                "fleq.s",
                pegasus::Action::createAction<&RvzfaInsts::fliHandler_<RV64, FLOAT_SP>, RvzfaInsts>(
                    nullptr, "fleq.s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fli.s",
                pegasus::Action::createAction<&RvzfaInsts::fliHandler_<RV64, FLOAT_SP>, RvzfaInsts>(
                    nullptr, "fli.s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fltq.s",
                pegasus::Action::createAction<&RvzfaInsts::fliHandler_<RV64, FLOAT_SP>, RvzfaInsts>(
                    nullptr, "fltq.s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmaxm.s",
                pegasus::Action::createAction<&RvzfaInsts::fminmaxHandler_<RV64, FLOAT_SP, true>,
                                              RvzfaInsts>(nullptr, "fmaxm.s",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fminm.s",
                pegasus::Action::createAction<&RvzfaInsts::fminmaxHandler_<RV64, FLOAT_SP, false>,
                                              RvzfaInsts>(nullptr, "fminm.s",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fround.s",
                pegasus::Action::createAction<&RvzfaInsts::froundHandler_<RV64, FLOAT_SP, false>,
                                              RvzfaInsts>(nullptr, "fround.s",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "froundnx.s",
                pegasus::Action::createAction<&RvzfaInsts::froundHandler_<RV64, FLOAT_SP, true>,
                                              RvzfaInsts>(nullptr, "froundnx.s",
                                                          ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzfaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzfaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, typename FMT>
    Action::ItrType RvzfaInsts::fliHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = inst->getRs1();

        const FMT rd_val = fli_table_.at(rs1_val);
        WRITE_FP_REG<FLOAT_DP>(state, inst->getRd(), nanBoxing<FLOAT_DP, FMT>(rd_val));
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
        }
        else if constexpr (std::is_same_v<SIZE, FLOAT_DP>)
        {
            rd_val = f64_le_quiet(float64_t{rs1_val}, float64_t{rs2_val}) ? rs2_val : rs1_val;
        }

        fmaxFminNanCheck<SIZE>(rs1_val, rs2_val, rd_val, ISMAX);
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<FLOAT_DP, SIZE>(rd_val));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    // Currently only designed to support SP and DP
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
            WRITE_FP_REG<RV64>(
                state, inst->getRd(),
                nanBoxing<FLOAT_DP, SIZE>(
                    f64_roundToInt(float64_t{rs1_val}, softfloat_roundingMode, EXACT).v));
        }
        else
        {
            static_assert(true, "unsupported floating point size");
        }

        updateCsr<XLEN>(state);
        return ++action_it;
    }
} // namespace pegasus
