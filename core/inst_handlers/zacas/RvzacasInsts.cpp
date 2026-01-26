#include "core/inst_handlers/zacas/RvzacasInsts.hpp"
#include "core/PegasusState.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{

    template <typename XLEN>
    void RvzacasInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "amocas_w",
            pegasus::Action::createAction<
                &RvzacasInsts::computeAddressHandler_<XLEN, RvzacasInsts::AccessSize::Word>,
                RvzacasInsts>(nullptr, "amocas_w", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "amocas_d",
            pegasus::Action::createAction<
                &RvzacasInsts::computeAddressHandler_<XLEN, RvzacasInsts::AccessSize::Double>,
                RvzacasInsts>(nullptr, "amocas_d", ActionTags::COMPUTE_ADDR_TAG));

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "amocas_q",
                pegasus::Action::createAction<
                    &RvzacasInsts::computeAddressHandler_<XLEN, RvzacasInsts::AccessSize::Quadword>,
                    RvzacasInsts>(nullptr, "amocas_q", ActionTags::COMPUTE_ADDR_TAG));
        }
    }

    template <typename XLEN>
    void RvzacasInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "amocas_w",
            pegasus::Action::createAction<
                &RvzacasInsts::amocasHandler_<XLEN, RvzacasInsts::AccessSize::Word>, RvzacasInsts>(
                nullptr, "amocas_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "amocas_d", pegasus::Action::createAction<
                            &RvzacasInsts::amocasHandler_<XLEN, RvzacasInsts::AccessSize::Double>,
                            RvzacasInsts>(nullptr, "amocas_d", ActionTags::EXECUTE_TAG));

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "amocas_q",
                pegasus::Action::createAction<
                    &RvzacasInsts::amocasHandler_<XLEN, RvzacasInsts::AccessSize::Quadword>,
                    RvzacasInsts>(nullptr, "amocas_q", ActionTags::EXECUTE_TAG));
        }
    }

    template void
    RvzacasInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void
    RvzacasInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvzacasInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzacasInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN, RvzacasInsts::AccessSize accessSize>
    Action::ItrType RvzacasInsts::computeAddressHandler_(pegasus::PegasusState* state,
                                                         Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        sparta_assert(!(rs1_val % static_cast<size_t>(accessSize)),
                      "memory access is not naturally aligned.");
        inst->getTranslationState()->makeRequest(rs1_val, static_cast<size_t>(accessSize));
        return ++action_it;
    }

    template <typename XLEN, RvzacasInsts::AccessSize accessSize>
    Action::ItrType RvzacasInsts::amocasHandler_(PegasusState* state, Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        const PegasusInstPtr & inst = state->getCurrentInst();
        auto xlation_state = inst->getTranslationState();
        const uint64_t paddr = xlation_state->getResult().getPAddr();
        xlation_state->popResult();

        if constexpr (sizeof(XLEN) >= static_cast<size_t>(accessSize))
        {
            using AcessType = UintType<static_case<size_t>(accessSize) * 8>;

            const AcessType temp =
                state->readMemory<AcessType>(paddr, MemAccessSource::INSTRUCTION);
            const AcessType comp = READ_INT_REG<XLEN>(state, inst->getRd());
            const AcessType swap = READ_INT_REG<XLEN>(state, inst->getRs2());
            if (temp == comp)
            {
                state->writeMemory<AcessType>(paddr, swap, MemAccessSource::INSTRUCTION);
            }
            WRITE_INT_REG<XLEN>(state, inst->getRd(), signExtend<AccessType, XLEN>(temp));
        }
        else // amocas.q for RV64 and amocas.d for RV32
        {
            sparta_assert(!(rs2_val % 1), "rs2 value is not even.");
            sparta_assert(!(rd_val % 1), "rd value is not even.");
            const XLEN temp0 = state->readMemory < XLEN >> (paddr, MemAccessSource::INSTRUCTION);
            const XLEN temp1 =
                state->readMemory < XLEN >> (paddr + sizeof(XLEN), MemAccessSource::INSTRUCTION);
            const XLEN comp0 = inst->getRd() == 0 || ? 0 : READ_INT_REG<XLEN>(state, inst->getRd());
            const XLEN comp1 =
                inst->getRd() == 0 || ? 0 : READ_INT_REG<XLEN>(state, inst->getRd() + 1);
            const XLEN swap0 =
                inst->getRs2() == 0 || ? 0 : READ_INT_REG<XLEN>(state, inst->getRs2());
            const XLEN swap1 =
                inst->getRs2() == 0 || ? 0 : READ_INT_REG<XLEN>(state, inst->getRs2() + 1);
            if (temp0 == comp0 && temp1 == comp1)
            {
                state->writeMemory<XLEN>(paddr, swap0, MemAccessSource::INSTRUCTION);
                state->writeMemory<XLEN>(paddr + sizeof(XLEN), swap1, MemAccessSource::INSTRUCTION);
            }
            if (inst->getRd() != 0)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(), temp0);
                WRITE_INT_REG<XLEN>(state, inst->getRd() + 1, temp1);
            }
        }

        return ++action_it;
    }
} // namespace pegasus
