#include "core/inst_handlers/a/RvaInstsBase.hpp"
#include "core/inst_handlers/a/RvaFunctors.hpp"
#include "core/inst_handlers/i/RviFunctors.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
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
    Action::ItrType RvaInstsBase::amoHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        static_assert(std::is_same_v<SIZE, B> ||
	              std::is_same_v<SIZE, H> ||
		      std::is_same_v<SIZE, W> ||
		      std::is_same_v<SIZE, D>);
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
    Action::ItrType RvaInstsBase::lr_handler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        static_assert(std::is_same_v<SIZE, B> ||
	              std::is_same_v<SIZE, H> ||
		      std::is_same_v<SIZE, W> ||
		      std::is_same_v<SIZE, D>);

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
    Action::ItrType RvaInstsBase::sc_handler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        static_assert(std::is_same_v<SIZE, B> ||
	              std::is_same_v<SIZE, H> ||
		      std::is_same_v<SIZE, W> ||
		      std::is_same_v<SIZE, D>);

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

    template Action::ItrType RvaInstsBase::computeAddressHandler_<RV32>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::computeAddressHandler_<RV64>(PegasusState*,
                                                                        Action::ItrType);
    // amoadd.b, amoadd.h, ammoadd.w, amoadd.d
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, B, std::plus<B>>(PegasusState*,
                                                                              Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, H, std::plus<H>>(PegasusState*,
                                                                              Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, W, std::plus<W>>(PegasusState*,
                                                                              Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, D, std::plus<D>>(PegasusState*,
                                                                              Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, B, std::plus<B>>(PegasusState*,
                                                                              Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, H, std::plus<H>>(PegasusState*,
                                                                              Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, W, std::plus<W>>(PegasusState*,
                                                                              Action::ItrType);
    // amoand.b, amoand.h, ammoand.w, amoand.d
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, B, std::bit_and<B>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, H, std::bit_and<H>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, W, std::bit_and<W>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, D, std::bit_and<D>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, B, std::bit_and<B>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, H, std::bit_and<H>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, W, std::bit_and<W>>(PegasusState*,
                                                                                 Action::ItrType);
    // amomaxu.b, amomaxu.h, ammomaxu.w, amomaxu.d
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV64, B, Max<std::make_signed_t<B>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV64, H, Max<std::make_signed_t<H>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV64, W, Max<std::make_signed_t<W>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV64, D, Max<std::make_signed_t<D>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV32, B, Max<std::make_signed_t<B>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV32, H, Max<std::make_signed_t<H>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV32, W, Max<std::make_signed_t<W>>, false>(PegasusState*,
                                                                          Action::ItrType);
    // amomax.b, amomax.h, ammomax.w, amomax.d
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, B, Max<B>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, H, Max<H>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, W, Max<W>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, D, Max<D>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, B, Max<B>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, H, Max<H>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, W, Max<W>>(PegasusState*,
                                                                        Action::ItrType);
    // amominu.b, amominu.h, ammominu.w, amominu.d
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV64, B, Min<std::make_signed_t<B>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV64, H, Min<std::make_signed_t<H>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV64, W, Min<std::make_signed_t<W>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV64, D, Min<std::make_signed_t<D>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV32, B, Min<std::make_signed_t<B>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV32, H, Min<std::make_signed_t<H>>, false>(PegasusState*,
                                                                          Action::ItrType);
    template Action::ItrType
    RvaInstsBase::amoHandler_<RV32, W, Min<std::make_signed_t<W>>, false>(PegasusState*,
                                                                          Action::ItrType);
    // amomin.b, amomin.h, ammomin.w, amomin.d
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, B, Min<B>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, H, Min<H>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, W, Min<W>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, D, Min<D>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, B, Min<B>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, H, Min<H>>(PegasusState*,
                                                                        Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, W, Min<W>>(PegasusState*,
                                                                        Action::ItrType);
    // amoor.b, amoor.h, ammoor.w, amoor.d
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, B, std::bit_or<B>>(PegasusState*,
                                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, H, std::bit_or<H>>(PegasusState*,
                                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, W, std::bit_or<W>>(PegasusState*,
                                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, D, std::bit_or<D>>(PegasusState*,
                                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, B, std::bit_or<B>>(PegasusState*,
                                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, H, std::bit_or<H>>(PegasusState*,
                                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, W, std::bit_or<W>>(PegasusState*,
                                                                                Action::ItrType);
    // amoswap.b, amoswap.h, ammoswap.w, amoswap.d
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, B, Swap<B>>(PegasusState*,
                                                                         Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, H, Swap<H>>(PegasusState*,
                                                                         Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, W, Swap<W>>(PegasusState*,
                                                                         Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, D, Swap<D>>(PegasusState*,
                                                                         Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, B, Swap<B>>(PegasusState*,
                                                                         Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, H, Swap<H>>(PegasusState*,
                                                                         Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, W, Swap<W>>(PegasusState*,
                                                                         Action::ItrType);
    // amoxor.b, amoxor.h, ammoxor.w, amoxor.d
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, B, std::bit_xor<B>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, H, std::bit_xor<H>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, W, std::bit_xor<W>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV64, D, std::bit_xor<D>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, B, std::bit_xor<B>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, H, std::bit_xor<H>>(PegasusState*,
                                                                                 Action::ItrType);
    template Action::ItrType RvaInstsBase::amoHandler_<RV32, W, std::bit_xor<W>>(PegasusState*,
                                                                                 Action::ItrType);
    // lr.b, lr.h, lr.w, lr.d
    template Action::ItrType RvaInstsBase::lr_handler_<RV64, B>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::lr_handler_<RV64, H>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::lr_handler_<RV64, W>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::lr_handler_<RV64, D>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::lr_handler_<RV32, B>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::lr_handler_<RV32, H>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::lr_handler_<RV32, W>(pegasus::PegasusState*,
                                                                Action::ItrType);
    // sc.b, sc.h, sc.w, sc.d
    template Action::ItrType RvaInstsBase::sc_handler_<RV64, B>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::sc_handler_<RV64, H>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::sc_handler_<RV64, W>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::sc_handler_<RV64, D>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::sc_handler_<RV32, B>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::sc_handler_<RV32, H>(pegasus::PegasusState*,
                                                                Action::ItrType);
    template Action::ItrType RvaInstsBase::sc_handler_<RV32, W>(pegasus::PegasusState*,
                                                                Action::ItrType);
} // namespace pegasus
