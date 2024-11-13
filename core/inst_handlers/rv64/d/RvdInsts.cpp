#include "core/inst_handlers/rv64/d/RvdInsts.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    void RvdInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace(
            "fld", atlas::Action::createAction<&RvdInsts::fld_64_compute_address_handler, RvdInsts>(
                       nullptr, "fld", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "flw", atlas::Action::createAction<&RvdInsts::flw_64_compute_address_handler, RvdInsts>(
                       nullptr, "flw", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "fsd", atlas::Action::createAction<&RvdInsts::fsd_64_compute_address_handler, RvdInsts>(
                       nullptr, "fsd", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "fsw", atlas::Action::createAction<&RvdInsts::fsw_64_compute_address_handler, RvdInsts>(
                       nullptr, "fsw", ActionTags::COMPUTE_ADDR_TAG));
    }

    void RvdInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace("fadd.d",
                              atlas::Action::createAction<&RvdInsts::fadd_d_64_handler, RvdInsts>(
                                  nullptr, "fadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fclass.d",
                              atlas::Action::createAction<&RvdInsts::fclass_d_64_handler, RvdInsts>(
                                  nullptr, "fclass_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.d.l",
                              atlas::Action::createAction<&RvdInsts::fcvt_d_l_64_handler, RvdInsts>(
                                  nullptr, "fcvt_d_l", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.d.lu", atlas::Action::createAction<&RvdInsts::fcvt_d_lu_64_handler, RvdInsts>(
                             nullptr, "fcvt_d_lu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.d.s",
                              atlas::Action::createAction<&RvdInsts::fcvt_d_s_64_handler, RvdInsts>(
                                  nullptr, "fcvt_d_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.d.w",
                              atlas::Action::createAction<&RvdInsts::fcvt_d_w_64_handler, RvdInsts>(
                                  nullptr, "fcvt_d_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.d.wu", atlas::Action::createAction<&RvdInsts::fcvt_d_wu_64_handler, RvdInsts>(
                             nullptr, "fcvt_d_wu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.l.d",
                              atlas::Action::createAction<&RvdInsts::fcvt_l_d_64_handler, RvdInsts>(
                                  nullptr, "fcvt_l_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.lu.d", atlas::Action::createAction<&RvdInsts::fcvt_lu_d_64_handler, RvdInsts>(
                             nullptr, "fcvt_lu_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.s.d",
                              atlas::Action::createAction<&RvdInsts::fcvt_s_d_64_handler, RvdInsts>(
                                  nullptr, "fcvt_s_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.w.d",
                              atlas::Action::createAction<&RvdInsts::fcvt_w_d_64_handler, RvdInsts>(
                                  nullptr, "fcvt_w_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.wu.d", atlas::Action::createAction<&RvdInsts::fcvt_wu_d_64_handler, RvdInsts>(
                             nullptr, "fcvt_wu_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fdiv.d",
                              atlas::Action::createAction<&RvdInsts::fdiv_d_64_handler, RvdInsts>(
                                  nullptr, "fdiv_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("feq.d",
                              atlas::Action::createAction<&RvdInsts::feq_d_64_handler, RvdInsts>(
                                  nullptr, "feq_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fld",
                              atlas::Action::createAction<&RvdInsts::fld_64_handler, RvdInsts>(
                                  nullptr, "fld", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fle.d",
                              atlas::Action::createAction<&RvdInsts::fle_d_64_handler, RvdInsts>(
                                  nullptr, "fle_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("flt.d",
                              atlas::Action::createAction<&RvdInsts::flt_d_64_handler, RvdInsts>(
                                  nullptr, "flt_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("flw",
                              atlas::Action::createAction<&RvdInsts::flw_64_handler, RvdInsts>(
                                  nullptr, "flw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmadd.d",
                              atlas::Action::createAction<&RvdInsts::fmadd_d_64_handler, RvdInsts>(
                                  nullptr, "fmadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmax.d",
                              atlas::Action::createAction<&RvdInsts::fmax_d_64_handler, RvdInsts>(
                                  nullptr, "fmax_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmin.d",
                              atlas::Action::createAction<&RvdInsts::fmin_d_64_handler, RvdInsts>(
                                  nullptr, "fmin_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmsub.d",
                              atlas::Action::createAction<&RvdInsts::fmsub_d_64_handler, RvdInsts>(
                                  nullptr, "fmsub_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmul.d",
                              atlas::Action::createAction<&RvdInsts::fmul_d_64_handler, RvdInsts>(
                                  nullptr, "fmul_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmv.d.x",
                              atlas::Action::createAction<&RvdInsts::fmv_d_x_64_handler, RvdInsts>(
                                  nullptr, "fmv_d_x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmv.w.x",
                              atlas::Action::createAction<&RvdInsts::fmv_w_x_64_handler, RvdInsts>(
                                  nullptr, "fmv_w_x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmv.x.d",
                              atlas::Action::createAction<&RvdInsts::fmv_x_d_64_handler, RvdInsts>(
                                  nullptr, "fmv_x_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmv.x.w",
                              atlas::Action::createAction<&RvdInsts::fmv_x_w_64_handler, RvdInsts>(
                                  nullptr, "fmv_x_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fnmadd.d",
                              atlas::Action::createAction<&RvdInsts::fnmadd_d_64_handler, RvdInsts>(
                                  nullptr, "fnmadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fnmsub.d",
                              atlas::Action::createAction<&RvdInsts::fnmsub_d_64_handler, RvdInsts>(
                                  nullptr, "fnmsub_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsd",
                              atlas::Action::createAction<&RvdInsts::fsd_64_handler, RvdInsts>(
                                  nullptr, "fsd", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsgnj.d",
                              atlas::Action::createAction<&RvdInsts::fsgnj_d_64_handler, RvdInsts>(
                                  nullptr, "fsgnj_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsgnjn.d",
                              atlas::Action::createAction<&RvdInsts::fsgnjn_d_64_handler, RvdInsts>(
                                  nullptr, "fsgnjn_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsgnjx.d",
                              atlas::Action::createAction<&RvdInsts::fsgnjx_d_64_handler, RvdInsts>(
                                  nullptr, "fsgnjx_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsqrt.d",
                              atlas::Action::createAction<&RvdInsts::fsqrt_d_64_handler, RvdInsts>(
                                  nullptr, "fsqrt_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsub.d",
                              atlas::Action::createAction<&RvdInsts::fsub_d_64_handler, RvdInsts>(
                                  nullptr, "fsub_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsw",
                              atlas::Action::createAction<&RvdInsts::fsw_64_handler, RvdInsts>(
                                  nullptr, "fsw", ActionTags::EXECUTE_TAG));
    }
}; // namespace atlas
