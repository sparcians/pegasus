#include "core/inst_handlers/zfh/RvzfhInsts.hpp"
#include "core/inst_handlers/f/RvfFunctors.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"

constexpr uint16_t hp_sign_mask = 1 << 15;

namespace pegasus
{
    template <typename XLEN>
    void RvzfhInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "flh",
            pegasus::Action::createAction<&RvzfhInsts::computeAddressHandler<XLEN>, RvfInstsBase>(
                nullptr, "flh", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "fsh",
            pegasus::Action::createAction<&RvzfhInsts::computeAddressHandler<XLEN>, RvfInstsBase>(
                nullptr, "fsh", ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvzfhInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "flh",
            pegasus::Action::createAction<&RvzfhInsts::floatLsHandler<FLOAT_HP, true>,
                                          RvfInstsBase>(nullptr, "flh", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsh",
            pegasus::Action::createAction<&RvzfhInsts::floatLsHandler<FLOAT_HP, false>,
                                          RvfInstsBase>(nullptr, "fsh", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "fadd_h", pegasus::Action::createAction<&RvzfhInsts::fadd_hHandler_<XLEN>, RvzfhInsts>(
                          nullptr, "fadd_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsub_h", pegasus::Action::createAction<&RvzfhInsts::fsub_hHandler_<XLEN>, RvzfhInsts>(
                          nullptr, "fsub_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmadd_h",
            pegasus::Action::createAction<&RvzfhInsts::fmadd_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fmadd_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmsub_h",
            pegasus::Action::createAction<&RvzfhInsts::fmsub_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fmsub_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmul_h", pegasus::Action::createAction<&RvzfhInsts::fmul_hHandler_<XLEN>, RvzfhInsts>(
                          nullptr, "fmul_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fdiv_h", pegasus::Action::createAction<&RvzfhInsts::fdiv_hHandler_<XLEN>, RvzfhInsts>(
                          nullptr, "fdiv_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmadd_h",
            pegasus::Action::createAction<&RvzfhInsts::fnmadd_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fnmadd_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmsub_h",
            pegasus::Action::createAction<&RvzfhInsts::fnmsub_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fnmsub_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsqrt_h",
            pegasus::Action::createAction<&RvzfhInsts::fsqrt_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fsqrt_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmax_h", pegasus::Action::createAction<&RvzfhInsts::fmax_hHandler_<XLEN>, RvzfhInsts>(
                          nullptr, "fmax_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmin_h", pegasus::Action::createAction<&RvzfhInsts::fmin_hHandler_<XLEN>, RvzfhInsts>(
                          nullptr, "fmin_h", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "fcvt_h_s",
            pegasus::Action::createAction<&RvzfhInsts::fcvt_h_sHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fcvt_h_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_h_w",
            pegasus::Action::createAction<&RvzfhInsts::fcvt_h_wHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fcvt_h_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_h_wu",
            pegasus::Action::createAction<&RvzfhInsts::fcvt_h_wuHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fcvt_h_wu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_s_h",
            pegasus::Action::createAction<&RvzfhInsts::fcvt_s_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fcvt_s_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_w_h",
            pegasus::Action::createAction<&RvzfhInsts::fcvt_w_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fcvt_w_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_wu_h",
            pegasus::Action::createAction<&RvzfhInsts::fcvt_wu_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fcvt_wu_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_d_h",
            pegasus::Action::createAction<&RvzfhInsts::fcvt_d_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fcvt_d_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_h_d",
            pegasus::Action::createAction<&RvzfhInsts::fcvt_h_dHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fcvt_h_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmv_h_x",
            pegasus::Action::createAction<&RvzfhInsts::fmv_h_xHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fmv_h_x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmv_x_h",
            pegasus::Action::createAction<&RvzfhInsts::fmv_x_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fmv_x_h", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "fsgnj_h",
            pegasus::Action::createAction<&RvzfhInsts::fsgnj_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fsgnj_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjn_h",
            pegasus::Action::createAction<&RvzfhInsts::fsgnjn_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fsgnjn_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjx_h",
            pegasus::Action::createAction<&RvzfhInsts::fsgnjx_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fsgnjx_h", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "feq_h", pegasus::Action::createAction<&RvzfhInsts::feq_hHandler_<XLEN>, RvzfhInsts>(
                         nullptr, "feq_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fle_h", pegasus::Action::createAction<&RvzfhInsts::fle_hHandler_<XLEN>, RvzfhInsts>(
                         nullptr, "fle_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "flt_h", pegasus::Action::createAction<&RvzfhInsts::flt_hHandler_<XLEN>, RvzfhInsts>(
                         nullptr, "flt_h", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "fclass_h",
            pegasus::Action::createAction<&RvzfhInsts::fclass_hHandler_<XLEN>, RvzfhInsts>(
                nullptr, "fclass_h", ActionTags::EXECUTE_TAG));
        if constexpr (sizeof(XLEN) >= sizeof(FLOAT_DP))
        {
            inst_handlers.emplace(
                "fcvt_h_l",
                pegasus::Action::createAction<&RvzfhInsts::fcvt_h_lHandler_<XLEN>, RvzfhInsts>(
                    nullptr, "fcvt_h_l", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt_h_lu",
                pegasus::Action::createAction<&RvzfhInsts::fcvt_h_luHandler_<XLEN>, RvzfhInsts>(
                    nullptr, "fcvt_h_lu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt_l_h",
                pegasus::Action::createAction<&RvzfhInsts::fcvt_l_hHandler_<XLEN>, RvzfhInsts>(
                    nullptr, "fcvt_l_h", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt_lu_h",
                pegasus::Action::createAction<&RvzfhInsts::fcvt_lu_hHandler_<XLEN>, RvzfhInsts>(
                    nullptr, "fcvt_lu_h", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzfhInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzfhInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvzfhInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzfhInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fsqrt_hHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, FLOAT_HP>(f16_sqrt(float16_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fsub_hHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, FLOAT_HP>(f16_sub(float16_t{rs1_val}, float16_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fnmsub_hHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const FLOAT_HP rs3_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs3()));
        const RV64 result = nanBoxing<RV64, FLOAT_HP>(
            Fnmsub<float16_t>{}(float16_t{rs1_val}, float16_t{rs2_val}, float16_t{rs3_val}).v);
        WRITE_FP_REG<RV64>(state, inst->getRd(), result);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fmsub_hHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const FLOAT_HP rs3_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs3()));
        const RV64 result = nanBoxing<RV64, FLOAT_HP>(
            Fmsub<float16_t>{}(float16_t{rs1_val}, float16_t{rs2_val}, float16_t{rs3_val}).v);
        WRITE_FP_REG<RV64>(state, inst->getRd(), result);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fdiv_hHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, FLOAT_HP>(f16_div(float16_t{rs1_val}, float16_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::feq_hHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f16_eq(float16_t{rs1_val}, float16_t{rs2_val}));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fmin_hHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        FLOAT_HP rd_val = f16_le_quiet(float16_t{rs1_val}, float16_t{rs2_val}) ? rs1_val : rs2_val;
        fmaxFminNanZeroCheck<FLOAT_HP>(rs1_val, rs2_val, rd_val, false);
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, FLOAT_HP>(rd_val));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fmv_h_xHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, FLOAT_HP>(rs1_val));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_lu_hHandler_(pegasus::PegasusState* state,
                                                  Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f16_to_ui64(float16_t{rs1_val}, getRM<XLEN>(state), true));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_h_wHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, FLOAT_HP>(i32_to_f16(rs1_val).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fnmadd_hHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const FLOAT_HP rs3_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs3()));
        const RV64 result = nanBoxing<RV64, FLOAT_HP>(
            Fnmadd<float16_t>{}(float16_t{rs1_val}, float16_t{rs2_val}, float16_t{rs3_val}).v);
        WRITE_FP_REG<RV64>(state, inst->getRd(), result);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_h_lHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, FLOAT_HP>(i64_to_f16(rs1_val).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fadd_hHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, FLOAT_HP>(f16_add(float16_t{rs1_val}, float16_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fmv_x_hHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), sext<XLEN, FLOAT_HP>(rs1_val));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fmax_hHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        FLOAT_HP rd_val = f16_le_quiet(float16_t{rs1_val}, float16_t{rs2_val}) ? rs2_val : rs1_val;
        fmaxFminNanZeroCheck<FLOAT_HP>(rs1_val, rs2_val, rd_val, true);
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, FLOAT_HP>(rd_val));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fsgnjx_hHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, FLOAT_HP>((rs1_val & ~hp_sign_mask)
                                                     | ((rs1_val ^ rs2_val) & hp_sign_mask)));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fmadd_hHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const FLOAT_HP rs3_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs3()));
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, FLOAT_HP>(
                Fmadd<float16_t>{}(float16_t{rs1_val}, float16_t{rs2_val}, float16_t{rs3_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fmul_hHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, FLOAT_HP>(f16_mul(float16_t{rs1_val}, float16_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::flt_hHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f16_lt(float16_t{rs1_val}, float16_t{rs2_val}));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_w_hHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(
            state, inst->getRd(),
            sext<XLEN, FLOAT_SP>(f16_to_i32(float16_t{rs1_val}, getRM<XLEN>(state), true)));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_l_hHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f16_to_i64(float16_t{rs1_val}, getRM<XLEN>(state), true));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fsgnjn_hHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, FLOAT_HP>((rs1_val & ~hp_sign_mask)
                                                     | ((rs2_val & hp_sign_mask) ^ hp_sign_mask)));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_h_luHandler_(pegasus::PegasusState* state,
                                                  Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, FLOAT_HP>(ui64_to_f16(rs1_val).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_wu_hHandler_(pegasus::PegasusState* state,
                                                  Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(
            state, inst->getRd(),
            sext<XLEN, FLOAT_SP>(f16_to_ui32(float16_t{rs1_val}, getRM<XLEN>(state), true)));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fsgnj_hHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, FLOAT_HP>((rs1_val & ~hp_sign_mask) | (rs2_val & hp_sign_mask)));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_h_wuHandler_(pegasus::PegasusState* state,
                                                  Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, FLOAT_HP>(ui32_to_f16(rs1_val).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fle_hHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const FLOAT_HP rs2_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f16_le(float16_t{rs1_val}, float16_t{rs2_val}));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fclass_hHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));

        WRITE_INT_REG<XLEN>(state, inst->getRd(), fclass(rs1_val));

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_h_sHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_SP rs1_val =
            checkNanBoxing<RV64, FLOAT_SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        softfloat_roundingMode = getRM<XLEN>(state);
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, FLOAT_HP>(f32_to_f16(float32_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_s_hHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, FLOAT_SP>(f16_to_f32(float16_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_h_dHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_DP rs1_val =
            checkNanBoxing<RV64, FLOAT_DP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        softfloat_roundingMode = getRM<XLEN>(state);
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, FLOAT_HP>(f64_to_f16(float64_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfhInsts::fcvt_d_hHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_HP rs1_val =
            checkNanBoxing<RV64, FLOAT_HP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, FLOAT_DP>(f16_to_f64(float16_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }
} // namespace pegasus
