#include "core/inst_handlers/h/RvhInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvhInsts::getInstComputeAddressHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "hlv.b",
            Action::createAction<
                &RvhInsts::computeAddressHandler_<XLEN, uint8_t, translate_types::AccessType::LOAD>,
                RvhInsts>(nullptr, "hlv.b", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlv.bu",
            Action::createAction<
                &RvhInsts::computeAddressHandler_<XLEN, uint8_t, translate_types::AccessType::LOAD>,
                RvhInsts>(nullptr, "hlv.bu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlv.h",
            Action::createAction<&RvhInsts::computeAddressHandler_<
                                     XLEN, uint16_t, translate_types::AccessType::LOAD>,
                                 RvhInsts>(nullptr, "hlv.h", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlv.hu",
            Action::createAction<&RvhInsts::computeAddressHandler_<
                                     XLEN, uint16_t, translate_types::AccessType::LOAD>,
                                 RvhInsts>(nullptr, "hlv.hu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlv.w",
            Action::createAction<&RvhInsts::computeAddressHandler_<
                                     XLEN, uint32_t, translate_types::AccessType::LOAD>,
                                 RvhInsts>(nullptr, "hlv.w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlvx.hu",
            Action::createAction<&RvhInsts::computeAddressHandler_<
                                     XLEN, uint16_t, translate_types::AccessType::EXECUTE>,
                                 RvhInsts>(nullptr, "hlvx.hu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlvx.wu",
            Action::createAction<&RvhInsts::computeAddressHandler_<
                                     XLEN, uint32_t, translate_types::AccessType::EXECUTE>,
                                 RvhInsts>(nullptr, "hlvx.wu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hsv.b",
            Action::createAction<&RvhInsts::computeAddressHandler_<
                                     XLEN, uint8_t, translate_types::AccessType::STORE>,
                                 RvhInsts>(nullptr, "hsv.b", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hsv.h",
            Action::createAction<&RvhInsts::computeAddressHandler_<
                                     XLEN, uint16_t, translate_types::AccessType::STORE>,
                                 RvhInsts>(nullptr, "hsv.h", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hsv.w",
            Action::createAction<&RvhInsts::computeAddressHandler_<
                                     XLEN, uint32_t, translate_types::AccessType::STORE>,
                                 RvhInsts>(nullptr, "hsv.w", ActionTags::COMPUTE_ADDR_TAG));

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "hlv.wu",
                Action::createAction<&RvhInsts::computeAddressHandler_<
                                         XLEN, uint32_t, translate_types::AccessType::LOAD>,
                                     RvhInsts>(nullptr, "hlv.wu", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "hlv.d",
                Action::createAction<&RvhInsts::computeAddressHandler_<
                                         XLEN, uint64_t, translate_types::AccessType::LOAD>,
                                     RvhInsts>(nullptr, "hlv.d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "hsv.d",
                Action::createAction<&RvhInsts::computeAddressHandler_<
                                         XLEN, uint64_t, translate_types::AccessType::STORE>,
                                     RvhInsts>(nullptr, "hsv.d", ActionTags::COMPUTE_ADDR_TAG));
        }
    }

    template void RvhInsts::getInstComputeAddressHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvhInsts::getInstComputeAddressHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN>
    void RvhInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        constexpr bool SIGN_EXTEND = true;

        inst_handlers.emplace(
            "hfence.vvma", Action::createAction<&RvhInsts::hfenceHandler_<XLEN, false>, RvhInsts>(
                               nullptr, "hfence.vvma", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("hfence.gvma",
                              Action::createAction<&RvhInsts::hfenceHandler_<XLEN, true>, RvhInsts>(
                                  nullptr, "hfence.gvma", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "hlv.b",
            Action::createAction<&RvhInsts::loadHandler_<XLEN, uint8_t, SIGN_EXTEND>, RvhInsts>(
                nullptr, "hlv.b", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlv.bu", Action::createAction<&RvhInsts::loadHandler_<XLEN, uint8_t>, RvhInsts>(
                          nullptr, "hlv.bu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlv.h",
            Action::createAction<&RvhInsts::loadHandler_<XLEN, uint16_t, SIGN_EXTEND>, RvhInsts>(
                nullptr, "hlv.h", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlv.hu", Action::createAction<&RvhInsts::loadHandler_<XLEN, uint16_t>, RvhInsts>(
                          nullptr, "hlv.hu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlvx.hu", Action::createAction<&RvhInsts::loadHandler_<XLEN, uint16_t>, RvhInsts>(
                           nullptr, "hlvx.hu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hlvx.wu", Action::createAction<&RvhInsts::loadHandler_<XLEN, uint32_t>, RvhInsts>(
                           nullptr, "hlvx.wu", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hsv.b", Action::createAction<&RvhInsts::storeHandler_<XLEN, uint8_t>, RvhInsts>(
                         nullptr, "hsv.b", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hsv.h", Action::createAction<&RvhInsts::storeHandler_<XLEN, uint16_t>, RvhInsts>(
                         nullptr, "hsv.h", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "hsv.w", Action::createAction<&RvhInsts::storeHandler_<XLEN, uint32_t>, RvhInsts>(
                         nullptr, "hsv.w", ActionTags::COMPUTE_ADDR_TAG));

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "hlv.w",
                Action::createAction<&RvhInsts::loadHandler_<XLEN, uint32_t, SIGN_EXTEND>,
                                     RvhInsts>(nullptr, "hlv.w", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "hlv.wu", Action::createAction<&RvhInsts::loadHandler_<XLEN, uint32_t>, RvhInsts>(
                              nullptr, "hlv.wu", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "hlv.d", Action::createAction<&RvhInsts::loadHandler_<XLEN, uint64_t>, RvhInsts>(
                             nullptr, "hlv.d", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "hsv.d", Action::createAction<&RvhInsts::storeHandler_<XLEN, uint64_t>, RvhInsts>(
                             nullptr, "hsv.d", ActionTags::COMPUTE_ADDR_TAG));
        }
        else
        {
            inst_handlers.emplace(
                "hlv.w", Action::createAction<&RvhInsts::loadHandler_<XLEN, uint32_t>, RvhInsts>(
                             nullptr, "hlv.w", ActionTags::COMPUTE_ADDR_TAG));
        }
    }

    template void RvhInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvhInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN, bool GUEST>
    Action::ItrType RvhInsts::hfenceHandler_(PegasusState* state, Action::ItrType action_it)
    {
        if (state->getVirtualMode())
        {
            THROW_ILLEGAL_VIRTUAL_INST;
        }

        const PrivMode priv_mode = state->getPrivMode();
        if constexpr (GUEST)
        {
            // HFENCE.GVMA is valid only in HS-mode when mstatus.TVM=0,
            // or in M-mode (irrespective of mstatus.TVM)
            XLEN tvm = READ_CSR_FIELD<XLEN>(state, MSTATUS, "tvm");
            if (((tvm == 0) && (priv_mode != PrivMode::HYPERVISOR))
                || (priv_mode != PrivMode::MACHINE))
            {
                THROW_ILLEGAL_VIRTUAL_INST;
            }
        }
        else
        {
            // hfence.vvma is only valid in M-mode or HS-mode
            if (priv_mode < PrivMode::HYPERVISOR)
            {
                THROW_ILLEGAL_VIRTUAL_INST;
            }
        }

        // TODO: Flush any TLBs and instruction/block caches in the future
        return ++action_it;
    }

    template <typename XLEN, typename SIZE, translate_types::AccessType ACCESS_TYPE>
    Action::ItrType RvhInsts::computeAddressHandler_(PegasusState* state, Action::ItrType action_it)
    {
        if (state->getVirtualMode())
        {
            THROW_ILLEGAL_VIRTUAL_INST;
        }

        // From Hypervisor spec:
        // instructions are valid only in M-mode or HS-mode, or in U-mode when hstatus.HU=1
        if ((state->getPrivMode() == PrivMode::USER) || READ_CSR_FIELD<XLEN>(state, HSTATUS, "hu"))
        {
            THROW_ILLEGAL_INST;
        }

        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN vaddr = READ_INT_REG<XLEN>(state, inst->getRs1());

        // const XLEN spvp_val = READ_CSR_FIELD(state, HSTATUS, "spvp");
        // const PrivMode priv_mode = spvp_val ? PrivMode::SUPERVISPOR : PrivMode::USER;

        inst->getTranslationState()->makeRequest(vaddr, sizeof(SIZE));
        return ++action_it;
    }

    template <typename XLEN, typename SIZE, bool SIGN_EXTEND>
    Action::ItrType RvhInsts::loadHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t paddr = inst->getTranslationState()->getResult().getPAddr();
        inst->getTranslationState()->popResult();
        if constexpr (SIGN_EXTEND)
        {
            const XLEN rd_val = signExtend<SIZE, XLEN>(state->readMemory<SIZE>(paddr));
            WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        }
        else
        {
            const XLEN rd_val = state->readMemory<SIZE>(paddr);
            WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        }
        return ++action_it;
    }

    template <typename XLEN, typename SIZE>
    Action::ItrType RvhInsts::storeHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        const uint64_t paddr = inst->getTranslationState()->getResult().getPAddr();
        inst->getTranslationState()->popResult();
        state->writeMemory<SIZE>(paddr, rs2_val);
        return ++action_it;
    }
} // namespace pegasus
