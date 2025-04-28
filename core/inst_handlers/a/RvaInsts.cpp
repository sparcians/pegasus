#include "core/inst_handlers/a/RvaInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename T> struct MaxFunctor
    {
        constexpr T operator()(const T & lhs, const T & rhs) const { return lhs > rhs ? lhs : rhs; }
    };

    template <typename T> struct MinFunctor
    {
        constexpr T operator()(const T & lhs, const T & rhs) const { return lhs > rhs ? rhs : lhs; }
    };

    template <typename T> struct SwapFunctor
    {
        constexpr T operator()(const T & lhs, const T & rhs) const
        {
            (void)lhs;
            return rhs;
        }
    };

    template <typename XLEN>
    void RvaInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "amoadd_w",
            atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                nullptr, "amoadd_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoand_w",
            atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                nullptr, "amoand_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomax_w",
            atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                nullptr, "amomax_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomaxu_w",
            atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                nullptr, "amomaxu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amomin_w",
            atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                nullptr, "amomin_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amominu_w",
            atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                nullptr, "amominu_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoor_w",
            atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                nullptr, "amoor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoswap_w",
            atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                nullptr, "amoswap_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amoxor_w",
            atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                nullptr, "amoxor_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "lr_w", atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                        nullptr, "lr_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sc_w", atlas::Action::createAction<&RvaInsts::compute_address_handler<XLEN>, RvaInsts>(
                        nullptr, "sc_w", ActionTags::COMPUTE_ADDR_TAG));

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "amoadd_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "amoadd_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoand_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "amoand_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomax_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "amomax_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomaxu_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "amomaxu_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amomin_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "amomin_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amominu_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "amominu_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoor_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "amoor_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoswap_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "amoswap_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "amoxor_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "amoxor_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lr_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "lr_d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sc_d",
                atlas::Action::createAction<&RvaInsts::compute_address_handler<RV64>, RvaInsts>(
                    nullptr, "sc_d", ActionTags::COMPUTE_ADDR_TAG));
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
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, D, std::plus<D>>,
                                            RvaInsts>(nullptr, "amoadd_d",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoadd_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, W, std::plus<W>>,
                                            RvaInsts>(nullptr, "amoadd_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand_d",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, D, std::bit_and<D>>,
                                            RvaInsts>(nullptr, "amoand_d",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, W, std::bit_and<W>>,
                                            RvaInsts>(nullptr, "amoand_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax_d",
                atlas::Action::createAction<
                    &RvaInsts::amo_handler<RV64, D, MaxFunctor<std::make_signed_t<D>>, false>,
                    RvaInsts>(nullptr, "amomax_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax_w",
                atlas::Action::createAction<
                    &RvaInsts::amo_handler<RV64, W, MaxFunctor<std::make_signed_t<W>>, false>,
                    RvaInsts>(nullptr, "amomax_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu_d",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, D, MaxFunctor<D>>,
                                            RvaInsts>(nullptr, "amomaxu_d",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, W, MaxFunctor<W>>,
                                            RvaInsts>(nullptr, "amomaxu_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin_d",
                atlas::Action::createAction<
                    &RvaInsts::amo_handler<RV64, D, MinFunctor<std::make_signed_t<D>>, false>,
                    RvaInsts>(nullptr, "amomin_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin_w",
                atlas::Action::createAction<
                    &RvaInsts::amo_handler<RV64, W, MinFunctor<std::make_signed_t<W>>, false>,
                    RvaInsts>(nullptr, "amomin_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu_d",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, D, MinFunctor<D>>,
                                            RvaInsts>(nullptr, "amominu_d",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, W, MinFunctor<W>>,
                                            RvaInsts>(nullptr, "amominu_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor_d",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, D, std::bit_or<D>>,
                                            RvaInsts>(nullptr, "amoor_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, W, std::bit_or<W>>,
                                            RvaInsts>(nullptr, "amoor_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap_d",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, D, SwapFunctor<D>>,
                                            RvaInsts>(nullptr, "amoswap_d",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, W, SwapFunctor<W>>,
                                            RvaInsts>(nullptr, "amoswap_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor_d",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, D, std::bit_xor<D>>,
                                            RvaInsts>(nullptr, "amoxor_d",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV64, W, std::bit_xor<W>>,
                                            RvaInsts>(nullptr, "amoxor_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lr_d",
                                  atlas::Action::createAction<&RvaInsts::lr_d_64_handler, RvaInsts>(
                                      nullptr, "lr_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lr_w",
                                  atlas::Action::createAction<&RvaInsts::lr_w_64_handler, RvaInsts>(
                                      nullptr, "lr_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sc_d",
                                  atlas::Action::createAction<&RvaInsts::sc_d_64_handler, RvaInsts>(
                                      nullptr, "sc_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sc_w",
                                  atlas::Action::createAction<&RvaInsts::sc_w_64_handler, RvaInsts>(
                                      nullptr, "sc_w", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "amoadd_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV32, W, std::plus<W>>,
                                            RvaInsts>(nullptr, "amoadd_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoand_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV32, W, std::bit_and<W>>,
                                            RvaInsts>(nullptr, "amoand_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomax_w",
                atlas::Action::createAction<
                    &RvaInsts::amo_handler<RV32, W, MaxFunctor<std::make_signed_t<W>>, false>,
                    RvaInsts>(nullptr, "amomax_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomaxu_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV32, W, MaxFunctor<W>>,
                                            RvaInsts>(nullptr, "amomaxu_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amomin_w",
                atlas::Action::createAction<
                    &RvaInsts::amo_handler<RV32, W, MinFunctor<std::make_signed_t<W>>, false>,
                    RvaInsts>(nullptr, "amomin_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amominu_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV32, W, MinFunctor<W>>,
                                            RvaInsts>(nullptr, "amominu_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoor_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV32, W, std::bit_or<W>>,
                                            RvaInsts>(nullptr, "amoor_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoswap_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV32, W, SwapFunctor<W>>,
                                            RvaInsts>(nullptr, "amoswap_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "amoxor_w",
                atlas::Action::createAction<&RvaInsts::amo_handler<RV32, W, std::bit_xor<W>>,
                                            RvaInsts>(nullptr, "amoxor_w",
                                                      ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lr_w",
                                  atlas::Action::createAction<&RvaInsts::lr_w_64_handler, RvaInsts>(
                                      nullptr, "lr_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sc_w",
                                  atlas::Action::createAction<&RvaInsts::sc_w_64_handler, RvaInsts>(
                                      nullptr, "sc_w", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvaInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvaInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvaInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvaInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename T> ActionGroup* RvaInsts::compute_address_handler(AtlasState* state)
    {
        static_assert(std::is_same_v<T, RV64> || std::is_same_v<T, RV32>);

        const AtlasInstPtr & inst = state->getCurrentInst();
        const T rs1_val = inst->getRs1Reg()->dmiRead<uint64_t>();
        const T imm = inst->getImmediate();
        const T vaddr = rs1_val + imm;
        inst->getTranslationState()->makeRequest(vaddr, sizeof(T));
        return nullptr;
    }

    template <typename RV, typename SIZE, typename OP, bool U>
    ActionGroup* RvaInsts::amo_handler(atlas::AtlasState* state)
    {
        static_assert(std::is_same_v<RV, RV64> || std::is_same_v<RV, RV32>);
        static_assert(std::is_same_v<SIZE, W> || std::is_same_v<SIZE, D>);
        static_assert(sizeof(RV) >= sizeof(SIZE));

        const AtlasInstPtr & inst = state->getCurrentInst();
        const RV paddr = inst->getTranslationState()->getResult().getPaddr();
        RV rd_val = 0;
        if constexpr (sizeof(RV) > sizeof(SIZE))
        {
            rd_val = signExtend<SIZE, RV>(state->readMemory<SIZE>(paddr));
        }
        else
        {
            rd_val = state->readMemory<SIZE>(paddr);
        }
        inst->getRdReg()->write(rd_val);
        const RV rs2_val = inst->getRs2Reg()->dmiRead<uint64_t>();
        state->writeMemory<SIZE>(paddr, OP()(rd_val, rs2_val));
        return nullptr;
    }

    ActionGroup* RvaInsts::lr_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t paddr = inst->getTranslationState()->getResult().getPaddr();
        const uint64_t rd_val = state->readMemory<uint64_t>(paddr);
        inst->getRdReg()->write(rd_val);
        return nullptr;
    }

    ActionGroup* RvaInsts::lr_w_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t paddr = inst->getTranslationState()->getResult().getPaddr();
        const uint64_t rd_val = signExtend<uint32_t, uint64_t>(state->readMemory<uint32_t>(paddr));
        inst->getRdReg()->write(rd_val);
        return nullptr;
    }

    ActionGroup* RvaInsts::sc_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs2_val = inst->getRs2Reg()->dmiRead<uint64_t>();
        const uint64_t paddr = inst->getTranslationState()->getResult().getPaddr();
        state->writeMemory<uint64_t>(paddr, rs2_val);
        return nullptr;
    }

    ActionGroup* RvaInsts::sc_w_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs2_val = inst->getRs2Reg()->dmiRead<uint64_t>();
        const uint64_t paddr = inst->getTranslationState()->getResult().getPaddr();
        state->writeMemory<uint32_t>(paddr, rs2_val);
        return nullptr;
    }
} // namespace atlas
