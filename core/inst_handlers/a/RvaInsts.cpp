#include "core/inst_handlers/a/RvaInsts.hpp"
#include "core/inst_handlers/a/RvaFunctors.hpp"
#include "core/inst_handlers/i/RviFunctors.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvaInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "amoadd_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "amoadd_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoand_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "amoand_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomax_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "amomax_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomaxu_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "amomaxu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomin_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "amomin_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amominu_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "amominu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoor_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "amoor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoswap_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "amoswap_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoxor_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "amoxor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lr_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "lr_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sc_w",
            pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<XLEN>, RvaInstsBase>(
                nullptr, "sc_w", ActionTags::COMPUTE_ADDR_TAG));

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "amoadd_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                          RvaInstsBase>(
                                nullptr, "amoadd_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoand_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                          RvaInstsBase>(
                                nullptr, "amoand_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomax_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                          RvaInstsBase>(
                                nullptr, "amomax_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomaxu_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                           RvaInstsBase>(
                                 nullptr, "amomaxu_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomin_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                          RvaInstsBase>(
                                nullptr, "amomin_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amominu_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                           RvaInstsBase>(
                                 nullptr, "amominu_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoor_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                         RvaInstsBase>(
                               nullptr, "amoor_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoswap_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                           RvaInstsBase>(
                                 nullptr, "amoswap_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoxor_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                          RvaInstsBase>(
                                nullptr, "amoxor_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lr_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                      RvaInstsBase>(nullptr, "lr_d",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sc_d", pegasus::Action::createAction<&RvaInsts::computeAddressHandler_<RV64>,
                                                      RvaInstsBase>(nullptr, "sc_d",
                                                                    ActionTags::COMPUTE_ADDR_TAG));
        }
    }

    template <typename XLEN>
    void RvaInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "amoadd_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, std::plus<D>>,
                                              RvaInstsBase>(nullptr, "amoadd_d",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoadd_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, std::plus<W>>,
                                              RvaInstsBase>(nullptr, "amoadd_w",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, std::bit_and<D>>,
                                              RvaInstsBase>(nullptr, "amoand_d",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, std::bit_and<W>>,
                                              RvaInstsBase>(nullptr, "amoand_w",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax_d", pegasus::Action::createAction<
                                &RvaInsts::amoHandler_<RV64, D, Max<std::make_signed_t<D>>, false>,
                                RvaInstsBase>(nullptr, "amomax_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax_w", pegasus::Action::createAction<
                                &RvaInsts::amoHandler_<RV64, W, Max<std::make_signed_t<W>>, false>,
                                RvaInstsBase>(nullptr, "amomax_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu_d", pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, Max<D>>,
                                                           RvaInstsBase>(nullptr, "amomaxu_d",
                                                                         ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu_w", pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, Max<W>>,
                                                           RvaInstsBase>(nullptr, "amomaxu_w",
                                                                         ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin_d", pegasus::Action::createAction<
                                &RvaInsts::amoHandler_<RV64, D, Min<std::make_signed_t<D>>, false>,
                                RvaInstsBase>(nullptr, "amomin_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin_w", pegasus::Action::createAction<
                                &RvaInsts::amoHandler_<RV64, W, Min<std::make_signed_t<W>>, false>,
                                RvaInstsBase>(nullptr, "amomin_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu_d", pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, Min<D>>,
                                                           RvaInstsBase>(nullptr, "amominu_d",
                                                                         ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu_w", pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, Min<W>>,
                                                           RvaInstsBase>(nullptr, "amominu_w",
                                                                         ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, std::bit_or<D>>,
                                              RvaInstsBase>(nullptr, "amoor_d",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, std::bit_or<W>>,
                                              RvaInstsBase>(nullptr, "amoor_w",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap_d", pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, Swap<D>>,
                                                           RvaInstsBase>(nullptr, "amoswap_d",
                                                                         ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap_w", pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, Swap<W>>,
                                                           RvaInstsBase>(nullptr, "amoswap_w",
                                                                         ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor_d",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, D, std::bit_xor<D>>,
                                              RvaInstsBase>(nullptr, "amoxor_d",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV64, W, std::bit_xor<W>>,
                                              RvaInstsBase>(nullptr, "amoxor_w",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lr_d",
                pegasus::Action::createAction<&RvaInsts::lr_handler_<RV64, uint64_t>, RvaInstsBase>(
                    nullptr, "lr_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lr_w",
                pegasus::Action::createAction<&RvaInsts::lr_handler_<RV64, uint32_t>, RvaInstsBase>(
                    nullptr, "lr_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sc_d",
                pegasus::Action::createAction<&RvaInsts::sc_handler_<RV64, uint64_t>, RvaInstsBase>(
                    nullptr, "sc_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sc_w",
                pegasus::Action::createAction<&RvaInsts::sc_handler_<RV64, uint32_t>, RvaInstsBase>(
                    nullptr, "sc_w", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "amoadd_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, std::plus<W>>,
                                              RvaInstsBase>(nullptr, "amoadd_w",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, std::bit_and<W>>,
                                              RvaInstsBase>(nullptr, "amoand_w",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax_w", pegasus::Action::createAction<
                                &RvaInsts::amoHandler_<RV32, W, Max<std::make_signed_t<W>>, false>,
                                RvaInstsBase>(nullptr, "amomax_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu_w", pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, Max<W>>,
                                                           RvaInstsBase>(nullptr, "amomaxu_w",
                                                                         ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin_w", pegasus::Action::createAction<
                                &RvaInsts::amoHandler_<RV32, W, Min<std::make_signed_t<W>>, false>,
                                RvaInstsBase>(nullptr, "amomin_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu_w", pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, Min<W>>,
                                                           RvaInstsBase>(nullptr, "amominu_w",
                                                                         ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, std::bit_or<W>>,
                                              RvaInstsBase>(nullptr, "amoor_w",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap_w", pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, Swap<W>>,
                                                           RvaInstsBase>(nullptr, "amoswap_w",
                                                                         ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor_w",
                pegasus::Action::createAction<&RvaInsts::amoHandler_<RV32, W, std::bit_xor<W>>,
                                              RvaInstsBase>(nullptr, "amoxor_w",
                                                            ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "lr_w",
                pegasus::Action::createAction<&RvaInsts::lr_handler_<RV32, uint32_t>, RvaInstsBase>(
                    nullptr, "lr_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sc_w",
                pegasus::Action::createAction<&RvaInsts::sc_handler_<RV32, uint32_t>, RvaInstsBase>(
                    nullptr, "sc_w", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvaInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvaInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);
} // namespace pegasus
