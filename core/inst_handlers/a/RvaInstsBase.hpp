#pragma once

#include "core/PegasusCore.hpp"
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
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
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
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
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
        const uint64_t paddr = xlation_state->getResult().getPAddr();
        state->getCore()->makeReservation(state->getHartId(), paddr);

        // Get the memory
        xlation_state->popResult();
        const XLEN rd_val = state->readMemory<SIZE>(paddr);
        if constexpr (sizeof(XLEN) > sizeof(SIZE))
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), signExtend<SIZE, XLEN>(rd_val));
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
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

        XLEN fail_code = 1; // assume bad
        if (const auto & resv = state->getCore()->getReservation(state->getHartId());
            resv.isValid())
        {
            if (resv == xlation_state->getResult().getPAddr())
            {
                const uint64_t paddr = xlation_state->getResult().getPAddr();
                const uint64_t rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
                state->writeMemory<SIZE>(paddr, rs2_val);
                fail_code = 0;
            }
        }
        xlation_state->popResult();

        // From RISC-V spec:
        //
        // Regardless of success or failure, executing an SC instruction invalidates
        // any reservation held by this hart.
        //
        // An SC may succeed only if no store from another hart to the
        // reservation set can be observed to have occurred between the LR and the SC,
        // and if there is no other SC between the LR and itself in program order.
        // FIXME: All store instructions can invalidate a reservation, not just a SC

        // Always clear all reservations after a SC
        state->getCore()->clearReservations();

        WRITE_INT_REG<XLEN>(state, inst->getRd(), fail_code);
        return ++action_it;
    }
} // namespace pegasus
