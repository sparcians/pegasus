#pragma once

#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"
#include "core/inst_handlers/a/RvaFunctors.hpp"
#include "core/inst_handlers/i/RviFunctors.hpp"

namespace pegasus
{
    class RvaInstsBase
    {
      public:
        using base_type = RvaInstsBase;

      protected:
        template <typename XLEN>
        Action::ItrType computeAddressHandler_(PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename SIZE, typename OP, bool U = true>
        Action::ItrType amoHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename SIZE>
        Action::ItrType lr_handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename SIZE>
        Action::ItrType sc_handler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };

    template <typename XLEN>
    Action::ItrType RvaInstsBase::computeAddressHandler_(PegasusState* state,
                                                         Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = inst->getRs1Reg()->dmiRead<uint64_t>();
        const XLEN imm = inst->getImmediate();
        const XLEN vaddr = rs1_val + imm;
        inst->getTranslationState()->makeRequest(vaddr, sizeof(XLEN));
        return ++action_it;
    }

    template <typename XLEN, typename SIZE, typename OP, bool U>
    Action::ItrType RvaInstsBase::amoHandler_(PegasusState* state, Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        static_assert(std::is_same_v<SIZE, B> || std::is_same_v<SIZE, H> || std::is_same_v<SIZE, W>
                      || std::is_same_v<SIZE, D>);
        static_assert(sizeof(XLEN) >= sizeof(SIZE));

        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN paddr = inst->getTranslationState()->getResult().getPAddr();
        inst->getTranslationState()->popResult();

        XLEN rd_val = 0;
        if constexpr (sizeof(XLEN) > sizeof(SIZE))
        {
            rd_val = signExtend<SIZE, XLEN>(state->readMemory<SIZE>(paddr));
        }
        else
        {
            rd_val = state->readMemory<SIZE>(paddr);
        }
        // Must read the RS2 value before writing the Rd (might be the
        // same register!)
        const XLEN rs2_val = inst->getRs2Reg()->dmiRead<uint64_t>();
        inst->getRdReg()->write(rd_val);
        state->writeMemory<SIZE>(paddr, OP()(rd_val, rs2_val));
        return ++action_it;
    }

    template <typename XLEN, typename SIZE>
    Action::ItrType RvaInstsBase::lr_handler_(PegasusState* state, Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        static_assert(std::is_same_v<SIZE, B> || std::is_same_v<SIZE, H> || std::is_same_v<SIZE, W>
                      || std::is_same_v<SIZE, D>);

        const PegasusInstPtr & inst = state->getCurrentInst();
        auto xlation_state = inst->getTranslationState();

        // Make the reservation
        state->getReservation() = xlation_state->getResult().getVAddr();

        // Get the memory
        const uint64_t paddr = xlation_state->getResult().getPAddr();
        xlation_state->popResult();
        const XLEN rd_val = state->readMemory<SIZE>(paddr);
        if constexpr (sizeof(XLEN) > sizeof(SIZE))
        {
            inst->getRdReg()->write(signExtend<SIZE, XLEN>(rd_val));
        }
        else
        {
            inst->getRdReg()->write(rd_val);
        }

        return ++action_it;
    }

    template <typename XLEN, typename SIZE>
    Action::ItrType RvaInstsBase::sc_handler_(PegasusState* state, Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        static_assert(std::is_same_v<SIZE, B> || std::is_same_v<SIZE, H> || std::is_same_v<SIZE, W>
                      || std::is_same_v<SIZE, D>);

        const PegasusInstPtr & inst = state->getCurrentInst();
        auto xlation_state = inst->getTranslationState();

        XLEN sc_bad = 1; // assume bad
        if (auto & resv = state->getReservation(); resv.isValid())
        {
            if (resv == xlation_state->getResult().getVAddr())
            {
                const uint64_t paddr = xlation_state->getResult().getPAddr();
                const uint64_t rs2_val = inst->getRs2Reg()->dmiRead<uint64_t>();
                state->writeMemory<SIZE>(paddr, rs2_val);
                sc_bad = 0;
            }
            resv.clearValid();
        }
        xlation_state->popResult();
        inst->getRdReg()->dmiWrite<XLEN>(sc_bad);
        return ++action_it;
    }
} // namespace pegasus
