#include "core/inst_handlers/zabha/RvzabhaInsts.hpp"
#include "core/inst_handlers/a/RvaInsts.hpp"
#include "core/inst_handlers/a/RvaFunctors.hpp"
#include "core/inst_handlers/i/RviFunctors.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzabhaInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "amoadd_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amoadd_b",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoand_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amoand_b",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomax_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amomax_b",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomaxu_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                       RvaInstsBase>(nullptr, "amomaxu_b",
                                                                     ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomin_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amomin_b",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amominu_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                       RvaInstsBase>(nullptr, "amominu_b",
                                                                     ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoor_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                     RvaInstsBase>(nullptr, "amoor_b",
                                                                   ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoswap_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                       RvaInstsBase>(nullptr, "amoswap_b",
                                                                     ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoxor_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amoxor_b",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lr_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                  RvaInstsBase>(nullptr, "lr_b",
                                                                ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sc_b", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                  RvaInstsBase>(nullptr, "sc_b",
                                                                ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoadd_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amoadd_h",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoand_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amoand_h",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomax_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amomax_h",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomaxu_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                       RvaInstsBase>(nullptr, "amomaxu_h",
                                                                     ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomin_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amomin_h",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amominu_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                       RvaInstsBase>(nullptr, "amominu_h",
                                                                     ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoor_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                     RvaInstsBase>(nullptr, "amoor_h",
                                                                   ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoswap_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                       RvaInstsBase>(nullptr, "amoswap_h",
                                                                     ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoxor_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                      RvaInstsBase>(nullptr, "amoxor_h",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lr_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                  RvaInstsBase>(nullptr, "lr_h",
                                                                ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sc_h", pegasus::Action::createAction<&RvzabhaInsts::computeAddressHandler_<XLEN>,
                                                  RvaInstsBase>(nullptr, "sc_h",
                                                                ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvzabhaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "amoadd_h",
            pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, H, std::plus<H>>,
                                          RvaInstsBase>(nullptr, "amoadd_h",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoadd_b",
            pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, B, std::plus<B>>,
                                          RvaInstsBase>(nullptr, "amoadd_b",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoand_h",
            pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, H, std::bit_and<H>>,
                                          RvaInstsBase>(nullptr, "amoand_h",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoand_b",
            pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, B, std::bit_and<B>>,
                                          RvaInstsBase>(nullptr, "amoand_b",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomax_h", pegasus::Action::createAction<
                            &RvzabhaInsts::amoHandler_<XLEN, H, Max<std::make_signed_t<H>>, false>,
                            RvaInstsBase>(nullptr, "amomax_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomax_b", pegasus::Action::createAction<
                            &RvzabhaInsts::amoHandler_<XLEN, B, Max<std::make_signed_t<B>>, false>,
                            RvaInstsBase>(nullptr, "amomax_b", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomaxu_h", pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, H, Max<H>>,
                                                       RvaInstsBase>(nullptr, "amomaxu_h",
                                                                     ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomaxu_b", pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, B, Max<B>>,
                                                       RvaInstsBase>(nullptr, "amomaxu_b",
                                                                     ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomin_h", pegasus::Action::createAction<
                            &RvzabhaInsts::amoHandler_<XLEN, H, Min<std::make_signed_t<H>>, false>,
                            RvaInstsBase>(nullptr, "amomin_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amomin_b", pegasus::Action::createAction<
                            &RvzabhaInsts::amoHandler_<XLEN, B, Min<std::make_signed_t<B>>, false>,
                            RvaInstsBase>(nullptr, "amomin_b", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amominu_h", pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, H, Min<H>>,
                                                       RvaInstsBase>(nullptr, "amominu_h",
                                                                     ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amominu_b", pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, B, Min<B>>,
                                                       RvaInstsBase>(nullptr, "amominu_b",
                                                                     ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoor_h",
            pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, H, std::bit_or<H>>,
                                          RvaInstsBase>(nullptr, "amoor_h",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoor_b",
            pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, B, std::bit_or<B>>,
                                          RvaInstsBase>(nullptr, "amoor_b",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoswap_h", pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, H, Swap<H>>,
                                                       RvaInstsBase>(nullptr, "amoswap_h",
                                                                     ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoswap_b", pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, B, Swap<B>>,
                                                       RvaInstsBase>(nullptr, "amoswap_b",
                                                                     ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoxor_h",
            pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, H, std::bit_xor<H>>,
                                          RvaInstsBase>(nullptr, "amoxor_h",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amoxor_b",
            pegasus::Action::createAction<&RvzabhaInsts::amoHandler_<XLEN, B, std::bit_xor<B>>,
                                          RvaInstsBase>(nullptr, "amoxor_b",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "lr_h",
            pegasus::Action::createAction<&RvzabhaInsts::lr_handler_<XLEN, uint64_t>, RvaInstsBase>(
                nullptr, "lr_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "lr_b",
            pegasus::Action::createAction<&RvzabhaInsts::lr_handler_<XLEN, uint32_t>, RvaInstsBase>(
                nullptr, "lr_b", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sc_h",
            pegasus::Action::createAction<&RvzabhaInsts::sc_handler_<XLEN, uint64_t>, RvaInstsBase>(
                nullptr, "sc_h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sc_b",
            pegasus::Action::createAction<&RvzabhaInsts::sc_handler_<XLEN, uint32_t>, RvaInstsBase>(
                nullptr, "sc_b", ActionTags::EXECUTE_TAG));
    }

    template void
    RvzabhaInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void
    RvzabhaInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvzabhaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzabhaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);
} // namespace pegasus
