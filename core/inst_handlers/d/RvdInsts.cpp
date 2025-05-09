#include "core/inst_handlers/d/RvdInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvdInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "fld",
            atlas::Action::createAction<&RvdInsts::computeAddressHandler<XLEN>, RvfInstsBase>(
                nullptr, "fld", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "fsd",
            atlas::Action::createAction<&RvdInsts::computeAddressHandler<XLEN>, RvfInstsBase>(
                nullptr, "fsd", ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvdInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "fadd_d", atlas::Action::createAction<&RvdInsts::fadd_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fclass_d", atlas::Action::createAction<&RvdInsts::fclass_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fclass_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_d_s", atlas::Action::createAction<&RvdInsts::fcvt_d_sHandler_<XLEN>, RvdInsts>(
                            nullptr, "fcvt_d_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_d_w", atlas::Action::createAction<&RvdInsts::fcvt_d_wHandler_<XLEN>, RvdInsts>(
                            nullptr, "fcvt_d_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_d_wu", atlas::Action::createAction<&RvdInsts::fcvt_d_wuHandler_<XLEN>, RvdInsts>(
                             nullptr, "fcvt_d_wu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_s_d", atlas::Action::createAction<&RvdInsts::fcvt_s_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fcvt_s_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_w_d", atlas::Action::createAction<&RvdInsts::fcvt_w_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fcvt_w_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_wu_d", atlas::Action::createAction<&RvdInsts::fcvt_wu_dHandler_<XLEN>, RvdInsts>(
                             nullptr, "fcvt_wu_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fdiv_d", atlas::Action::createAction<&RvdInsts::fdiv_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fdiv_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("feq_d",
                              atlas::Action::createAction<&RvdInsts::feq_dHandler_<XLEN>, RvdInsts>(
                                  nullptr, "feq_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fld", atlas::Action::createAction<&RvdInsts::floatLsHandler<DP, true>, RvfInstsBase>(
                       nullptr, "fld", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fle_d",
                              atlas::Action::createAction<&RvdInsts::fle_dHandler_<XLEN>, RvdInsts>(
                                  nullptr, "fle_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("flt_d",
                              atlas::Action::createAction<&RvdInsts::flt_dHandler_<XLEN>, RvdInsts>(
                                  nullptr, "flt_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmadd_d", atlas::Action::createAction<&RvdInsts::fmadd_dHandler_<XLEN>, RvdInsts>(
                           nullptr, "fmadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmax_d", atlas::Action::createAction<&RvdInsts::fmax_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fmax_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmin_d", atlas::Action::createAction<&RvdInsts::fmin_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fmin_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmsub_d", atlas::Action::createAction<&RvdInsts::fmsub_dHandler_<XLEN>, RvdInsts>(
                           nullptr, "fmsub_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmul_d", atlas::Action::createAction<&RvdInsts::fmul_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fmul_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmadd_d", atlas::Action::createAction<&RvdInsts::fnmadd_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fnmadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmsub_d", atlas::Action::createAction<&RvdInsts::fnmsub_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fnmsub_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsd", atlas::Action::createAction<&RvdInsts::floatLsHandler<DP, false>, RvfInstsBase>(
                       nullptr, "fsd", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnj_d", atlas::Action::createAction<&RvdInsts::fsgnj_dHandler_<XLEN>, RvdInsts>(
                           nullptr, "fsgnj_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjn_d", atlas::Action::createAction<&RvdInsts::fsgnjn_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fsgnjn_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjx_d", atlas::Action::createAction<&RvdInsts::fsgnjx_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fsgnjx_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsqrt_d", atlas::Action::createAction<&RvdInsts::fsqrt_dHandler_<XLEN>, RvdInsts>(
                           nullptr, "fsqrt_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsub_d", atlas::Action::createAction<&RvdInsts::fsub_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fsub_d", ActionTags::EXECUTE_TAG));
        if constexpr (sizeof(XLEN) >= sizeof(DP))
        {
            inst_handlers.emplace(
                "fcvt_d_l",
                atlas::Action::createAction<&RvdInsts::fcvt_d_lHandler_<XLEN>, RvdInsts>(
                    nullptr, "fcvt_d_l", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt_d_lu",
                atlas::Action::createAction<&RvdInsts::fcvt_d_luHandler_<XLEN>, RvdInsts>(
                    nullptr, "fcvt_d_lu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt_l_d",
                atlas::Action::createAction<&RvdInsts::fcvt_l_dHandler_<XLEN>, RvdInsts>(
                    nullptr, "fcvt_l_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt_lu_d",
                atlas::Action::createAction<&RvdInsts::fcvt_lu_dHandler_<XLEN>, RvdInsts>(
                    nullptr, "fcvt_lu_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv_d_x", atlas::Action::createAction<&RvdInsts::fmv_d_xHandler_<XLEN>, RvdInsts>(
                               nullptr, "fmv_d_x", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv_x_d", atlas::Action::createAction<&RvdInsts::fmv_x_dHandler_<XLEN>, RvdInsts>(
                               nullptr, "fmv_x_d", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvdInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvdInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvdInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvdInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN> Action* RvdInsts::fcvt_d_wHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), i32_to_f64(rs1_val).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fsub_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_sub(float64_t{rs1_val}, float64_t{rs2_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fmv_x_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rs1_val);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fcvt_wu_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f64_to_ui32(float64_t{rs1_val}, getRM<RV64>(state), true)));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fnmsub_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        const uint64_t product = f64_mul(float64_t{rs1_val}, float64_t{rs2_val}).v;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           f64_add(float64_t{product ^ (1UL << 63)}, float64_t{rs3_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fle_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f64_le(float64_t{rs1_val}, float64_t{rs2_val}));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fmul_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_mul(float64_t{rs1_val}, float64_t{rs2_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fsqrt_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_sqrt(float64_t{rs1_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fmadd_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            f64_add(f64_mul(float64_t{rs1_val}, float64_t{rs2_val}), float64_t{rs3_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fnmadd_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        const uint64_t product = f64_mul(float64_t{rs1_val}, float64_t{rs2_val}).v;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           f64_sub(float64_t{product ^ (1UL << 63)}, float64_t{rs3_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fmin_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        uint64_t rd_val = f64_le_quiet(float64_t{rs1_val}, float64_t{rs2_val}) ? rs1_val : rs2_val;
        fmaxFminNanZeroCheck<DP>(rs1_val, rs2_val, rd_val, false);
        WRITE_FP_REG<RV64>(state, inst->getRd(), rd_val);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fdiv_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_div(float64_t{rs1_val}, float64_t{rs2_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fsgnjx_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        static constexpr uint64_t sign_mask = 1UL << 63;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           (rs1_val & ~sign_mask) | ((rs1_val ^ rs2_val) & sign_mask));
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fmv_d_xHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), rs1_val);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fcvt_w_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f64_to_i32(float64_t{rs1_val}, getRM<RV64>(state), true)));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fcvt_lu_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f64_to_ui64(float64_t{rs1_val}, getRM<RV64>(state), true));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fsgnjn_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        static constexpr uint64_t sign_mask = 1UL << 63;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           (rs1_val & ~sign_mask) | ((rs2_val & sign_mask) ^ sign_mask));
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fclass_dHandler_(atlas::AtlasState* state, Action*)
    {
        state->getCurrentInst()->markUnimplemented();
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // WRITE_RD(f64_classify(FRS1_D));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fadd_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_add(float64_t{rs1_val}, float64_t{rs2_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fmsub_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            f64_sub(f64_mul(float64_t{rs1_val}, float64_t{rs2_val}), float64_t{rs3_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fcvt_d_sHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f32_to_f64(float32_t{rs1_val}).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fmax_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        uint64_t rd_val = f64_le_quiet(float64_t{rs1_val}, float64_t{rs2_val}) ? rs2_val : rs1_val;
        fmaxFminNanZeroCheck<DP>(rs1_val, rs2_val, rd_val, true);
        WRITE_FP_REG<RV64>(state, inst->getRd(), rd_val);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fcvt_s_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f64_to_f32(float64_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fcvt_d_luHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), ui64_to_f64(rs1_val).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::feq_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f64_eq(float64_t{rs1_val}, float64_t{rs2_val}));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fsgnj_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t sign_mask = 1UL << 63;
        WRITE_FP_REG<RV64>(state, inst->getRd(), (rs1_val & ~sign_mask) | (rs2_val & sign_mask));
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fcvt_d_lHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), i64_to_f64(rs1_val).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fcvt_d_wuHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), ui32_to_f64(rs1_val).v);
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::flt_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f64_lt(float64_t{rs1_val}, float64_t{rs2_val}));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> Action* RvdInsts::fcvt_l_dHandler_(atlas::AtlasState* state, Action*)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f64_to_i64(float64_t{rs1_val}, getRM<RV64>(state), true));
        updateCsr<XLEN>(state);
        return nullptr;
    }

} // namespace atlas
