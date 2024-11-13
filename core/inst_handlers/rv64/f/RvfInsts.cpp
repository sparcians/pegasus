#include "core/inst_handlers/rv64/f/RvfInsts.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    void RvfInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace(
            "flw", atlas::Action::createAction<&RvfInsts::flw_64_compute_address_handler, RvfInsts>(
                       nullptr, "flw", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "fsw", atlas::Action::createAction<&RvfInsts::fsw_64_compute_address_handler, RvfInsts>(
                       nullptr, "fsw", ActionTags::COMPUTE_ADDR_TAG));
    }

    void RvfInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace("fadd.s",
                              atlas::Action::createAction<&RvfInsts::fadd_s_64_handler, RvfInsts>(
                                  nullptr, "fadd_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fclass.s",
                              atlas::Action::createAction<&RvfInsts::fclass_s_64_handler, RvfInsts>(
                                  nullptr, "fclass_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.l.s",
                              atlas::Action::createAction<&RvfInsts::fcvt_l_s_64_handler, RvfInsts>(
                                  nullptr, "fcvt_l_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.lu.s", atlas::Action::createAction<&RvfInsts::fcvt_lu_s_64_handler, RvfInsts>(
                             nullptr, "fcvt_lu_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.s.l",
                              atlas::Action::createAction<&RvfInsts::fcvt_s_l_64_handler, RvfInsts>(
                                  nullptr, "fcvt_s_l", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.s.lu", atlas::Action::createAction<&RvfInsts::fcvt_s_lu_64_handler, RvfInsts>(
                             nullptr, "fcvt_s_lu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.s.w",
                              atlas::Action::createAction<&RvfInsts::fcvt_s_w_64_handler, RvfInsts>(
                                  nullptr, "fcvt_s_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.s.wu", atlas::Action::createAction<&RvfInsts::fcvt_s_wu_64_handler, RvfInsts>(
                             nullptr, "fcvt_s_wu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fcvt.w.s",
                              atlas::Action::createAction<&RvfInsts::fcvt_w_s_64_handler, RvfInsts>(
                                  nullptr, "fcvt_w_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.wu.s", atlas::Action::createAction<&RvfInsts::fcvt_wu_s_64_handler, RvfInsts>(
                             nullptr, "fcvt_wu_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fdiv.s",
                              atlas::Action::createAction<&RvfInsts::fdiv_s_64_handler, RvfInsts>(
                                  nullptr, "fdiv_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("feq.s",
                              atlas::Action::createAction<&RvfInsts::feq_s_64_handler, RvfInsts>(
                                  nullptr, "feq_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fle.s",
                              atlas::Action::createAction<&RvfInsts::fle_s_64_handler, RvfInsts>(
                                  nullptr, "fle_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("flt.s",
                              atlas::Action::createAction<&RvfInsts::flt_s_64_handler, RvfInsts>(
                                  nullptr, "flt_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("flw",
                              atlas::Action::createAction<&RvfInsts::flw_64_handler, RvfInsts>(
                                  nullptr, "flw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmadd.s",
                              atlas::Action::createAction<&RvfInsts::fmadd_s_64_handler, RvfInsts>(
                                  nullptr, "fmadd_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmax.s",
                              atlas::Action::createAction<&RvfInsts::fmax_s_64_handler, RvfInsts>(
                                  nullptr, "fmax_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmin.s",
                              atlas::Action::createAction<&RvfInsts::fmin_s_64_handler, RvfInsts>(
                                  nullptr, "fmin_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmsub.s",
                              atlas::Action::createAction<&RvfInsts::fmsub_s_64_handler, RvfInsts>(
                                  nullptr, "fmsub_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmul.s",
                              atlas::Action::createAction<&RvfInsts::fmul_s_64_handler, RvfInsts>(
                                  nullptr, "fmul_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmv.w.x",
                              atlas::Action::createAction<&RvfInsts::fmv_w_x_64_handler, RvfInsts>(
                                  nullptr, "fmv_w_x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fmv.x.w",
                              atlas::Action::createAction<&RvfInsts::fmv_x_w_64_handler, RvfInsts>(
                                  nullptr, "fmv_x_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fnmadd.s",
                              atlas::Action::createAction<&RvfInsts::fnmadd_s_64_handler, RvfInsts>(
                                  nullptr, "fnmadd_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fnmsub.s",
                              atlas::Action::createAction<&RvfInsts::fnmsub_s_64_handler, RvfInsts>(
                                  nullptr, "fnmsub_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsgnj.s",
                              atlas::Action::createAction<&RvfInsts::fsgnj_s_64_handler, RvfInsts>(
                                  nullptr, "fsgnj_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsgnjn.s",
                              atlas::Action::createAction<&RvfInsts::fsgnjn_s_64_handler, RvfInsts>(
                                  nullptr, "fsgnjn_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsgnjx.s",
                              atlas::Action::createAction<&RvfInsts::fsgnjx_s_64_handler, RvfInsts>(
                                  nullptr, "fsgnjx_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsqrt.s",
                              atlas::Action::createAction<&RvfInsts::fsqrt_s_64_handler, RvfInsts>(
                                  nullptr, "fsqrt_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsub.s",
                              atlas::Action::createAction<&RvfInsts::fsub_s_64_handler, RvfInsts>(
                                  nullptr, "fsub_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fsw",
                              atlas::Action::createAction<&RvfInsts::fsw_64_handler, RvfInsts>(
                                  nullptr, "fsw", ActionTags::EXECUTE_TAG));
    }
}; // namespace atlas
