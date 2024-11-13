#include "core/inst_handlers/rv64/a/RvaInsts.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    void RvaInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace(
            "amoadd.d",
            atlas::Action::createAction<&RvaInsts::amoadd_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoadd_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoadd.w",
            atlas::Action::createAction<&RvaInsts::amoadd_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoadd_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoand.d",
            atlas::Action::createAction<&RvaInsts::amoand_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoand_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoand.w",
            atlas::Action::createAction<&RvaInsts::amoand_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoand_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomax.d",
            atlas::Action::createAction<&RvaInsts::amomax_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amomax_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomax.w",
            atlas::Action::createAction<&RvaInsts::amomax_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amomax_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomaxu.d",
            atlas::Action::createAction<&RvaInsts::amomaxu_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amomaxu_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomaxu.w",
            atlas::Action::createAction<&RvaInsts::amomaxu_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amomaxu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomin.d",
            atlas::Action::createAction<&RvaInsts::amomin_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amomin_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomin.w",
            atlas::Action::createAction<&RvaInsts::amomin_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amomin_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amominu.d",
            atlas::Action::createAction<&RvaInsts::amominu_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amominu_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amominu.w",
            atlas::Action::createAction<&RvaInsts::amominu_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amominu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoor.d",
            atlas::Action::createAction<&RvaInsts::amoor_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoor_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoor.w",
            atlas::Action::createAction<&RvaInsts::amoor_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoswap.d",
            atlas::Action::createAction<&RvaInsts::amoswap_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoswap_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoswap.w",
            atlas::Action::createAction<&RvaInsts::amoswap_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoswap_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoxor.d",
            atlas::Action::createAction<&RvaInsts::amoxor_d_64_compute_address_handler, RvaInsts>(
                nullptr, "amoxor_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoxor.w",
            atlas::Action::createAction<&RvaInsts::amoxor_w_64_compute_address_handler, RvaInsts>(
                nullptr, "amoxor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lr.d",
            atlas::Action::createAction<&RvaInsts::lr_d_64_compute_address_handler, RvaInsts>(
                nullptr, "lr_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lr.w",
            atlas::Action::createAction<&RvaInsts::lr_w_64_compute_address_handler, RvaInsts>(
                nullptr, "lr_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sc.d",
            atlas::Action::createAction<&RvaInsts::sc_d_64_compute_address_handler, RvaInsts>(
                nullptr, "sc_d", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sc.w",
            atlas::Action::createAction<&RvaInsts::sc_w_64_compute_address_handler, RvaInsts>(
                nullptr, "sc_w", ActionTags::COMPUTE_ADDR_TAG));
    }

    void RvaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace("amoadd.d",
                              atlas::Action::createAction<&RvaInsts::amoadd_d_64_handler, RvaInsts>(
                                  nullptr, "amoadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoadd.w",
                              atlas::Action::createAction<&RvaInsts::amoadd_w_64_handler, RvaInsts>(
                                  nullptr, "amoadd_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoand.d",
                              atlas::Action::createAction<&RvaInsts::amoand_d_64_handler, RvaInsts>(
                                  nullptr, "amoand_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoand.w",
                              atlas::Action::createAction<&RvaInsts::amoand_w_64_handler, RvaInsts>(
                                  nullptr, "amoand_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amomax.d",
                              atlas::Action::createAction<&RvaInsts::amomax_d_64_handler, RvaInsts>(
                                  nullptr, "amomax_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amomax.w",
                              atlas::Action::createAction<&RvaInsts::amomax_w_64_handler, RvaInsts>(
                                  nullptr, "amomax_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomaxu.d", atlas::Action::createAction<&RvaInsts::amomaxu_d_64_handler, RvaInsts>(
                             nullptr, "amomaxu_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomaxu.w", atlas::Action::createAction<&RvaInsts::amomaxu_w_64_handler, RvaInsts>(
                             nullptr, "amomaxu_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amomin.d",
                              atlas::Action::createAction<&RvaInsts::amomin_d_64_handler, RvaInsts>(
                                  nullptr, "amomin_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amomin.w",
                              atlas::Action::createAction<&RvaInsts::amomin_w_64_handler, RvaInsts>(
                                  nullptr, "amomin_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amominu.d", atlas::Action::createAction<&RvaInsts::amominu_d_64_handler, RvaInsts>(
                             nullptr, "amominu_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amominu.w", atlas::Action::createAction<&RvaInsts::amominu_w_64_handler, RvaInsts>(
                             nullptr, "amominu_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoor.d",
                              atlas::Action::createAction<&RvaInsts::amoor_d_64_handler, RvaInsts>(
                                  nullptr, "amoor_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoor.w",
                              atlas::Action::createAction<&RvaInsts::amoor_w_64_handler, RvaInsts>(
                                  nullptr, "amoor_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoswap.d", atlas::Action::createAction<&RvaInsts::amoswap_d_64_handler, RvaInsts>(
                             nullptr, "amoswap_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoswap.w", atlas::Action::createAction<&RvaInsts::amoswap_w_64_handler, RvaInsts>(
                             nullptr, "amoswap_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoxor.d",
                              atlas::Action::createAction<&RvaInsts::amoxor_d_64_handler, RvaInsts>(
                                  nullptr, "amoxor_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("amoxor.w",
                              atlas::Action::createAction<&RvaInsts::amoxor_w_64_handler, RvaInsts>(
                                  nullptr, "amoxor_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lr.d",
                              atlas::Action::createAction<&RvaInsts::lr_d_64_handler, RvaInsts>(
                                  nullptr, "lr_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("lr.w",
                              atlas::Action::createAction<&RvaInsts::lr_w_64_handler, RvaInsts>(
                                  nullptr, "lr_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sc.d",
                              atlas::Action::createAction<&RvaInsts::sc_d_64_handler, RvaInsts>(
                                  nullptr, "sc_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("sc.w",
                              atlas::Action::createAction<&RvaInsts::sc_w_64_handler, RvaInsts>(
                                  nullptr, "sc_w", ActionTags::EXECUTE_TAG));
    }
}; // namespace atlas
