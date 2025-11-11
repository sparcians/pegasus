#include "core/inst_handlers/zcmp/RvzcmpInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/Trap.hpp"
#include "include/ActionTags.hpp"
#include "include/IntNums.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzcmpInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "cm.push", pegasus::Action::createAction<&RvzcmpInsts::pushHandler_<XLEN>, RvzcmpInsts>(
                           nullptr, "cm.push", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "cm.pop", pegasus::Action::createAction<&RvzcmpInsts::popHandler_<XLEN>, RvzcmpInsts>(
                          nullptr, "cm.pop", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "cm.popret",
            pegasus::Action::createAction<&RvzcmpInsts::popretHandler_<XLEN>, RvzcmpInsts>(
                nullptr, "cm.popret", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "cm.popretz",
            pegasus::Action::createAction<&RvzcmpInsts::popretzHandler_<XLEN>, RvzcmpInsts>(
                nullptr, "cm.popretz", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "cm.mva01s",
            pegasus::Action::createAction<&RvzcmpInsts::mva01sHandler_<XLEN>, RvzcmpInsts>(
                nullptr, "cm.mva01s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "cm.mvsa01",
            pegasus::Action::createAction<&RvzcmpInsts::mvsa01Handler_<XLEN>, RvzcmpInsts>(
                nullptr, "cm.mvsa01", ActionTags::EXECUTE_TAG));
    }

    template void RvzcmpInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzcmpInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN> void RvzcmpInsts::pop_(pegasus::PegasusState* state)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        // Load RA (1) and 0-12 saved registers (8-9, 18-27) from the stack frame
        // Have to start at idx 1 since Mavis includes the SP in the list of dests
        XLEN addr = READ_INT_REG<XLEN>(state, SP);
        const XLEN new_sp_val = addr + inst->getStackAdjustment();
        const auto & dst_reg_list = inst->getMavisOpcodeInfo()->getDestOpInfoList();
        for (uint32_t idx = 1; idx < dst_reg_list.size(); ++idx)
        {
            const auto dst = dst_reg_list[idx];
            addr -= sizeof(XLEN);
            const XLEN dst_reg_val = state->readMemory<XLEN>(addr);
            WRITE_INT_REG<XLEN>(state, dst.field_value, dst_reg_val);
        }

        // Update stack pointer
        WRITE_INT_REG<XLEN>(state, SP, new_sp_val);
    }

    template <typename XLEN>
    Action::ItrType RvzcmpInsts::pushHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        // Store RA (1) and 0-12 saved registers (8-9, 18-27) to the stack frame
        // Have to start at idx 1 since Mavis includes the SP in the list of sources
        XLEN addr = READ_INT_REG<XLEN>(state, SP);
        const XLEN new_sp_val = addr + inst->getStackAdjustment();
        const auto & src_reg_list = inst->getMavisOpcodeInfo()->getSourceOpInfoList();
        for (uint32_t idx = 1; idx < src_reg_list.size(); ++idx)
        {
            const auto src = src_reg_list[idx];
            addr -= sizeof(XLEN);
            const XLEN src_reg_val = READ_INT_REG<XLEN>(state, src.field_value);
            state->writeMemory<XLEN>(addr, src_reg_val);
        }

        // Update stack pointer
        WRITE_INT_REG<XLEN>(state, SP, new_sp_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzcmpInsts::popHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        pop_<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzcmpInsts::popretHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        pop_<XLEN>(state);

        // Set PC to RA (1)
        const XLEN ra_val = READ_INT_REG<XLEN>(state, 1);
        state->setNextPc(ra_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzcmpInsts::popretzHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        pop_<XLEN>(state);

        // Set PC to RA (1)
        const XLEN ra_val = READ_INT_REG<XLEN>(state, 1);
        state->setNextPc(ra_val);

        // Set A0 (10) to zero
        WRITE_INT_REG<XLEN>(state, 10, 0);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzcmpInsts::mva01sHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        // Write RS1 to A0 (10)
        WRITE_INT_REG<XLEN>(state, 10, rs1_val);
        // Write RS2 to A1 (11)
        WRITE_INT_REG<XLEN>(state, 11, rs2_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzcmpInsts::mvsa01Handler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN a0_val = READ_INT_REG<XLEN>(state, 10);
        const XLEN a1_val = READ_INT_REG<XLEN>(state, 11);

        // FIXME: Does Mavis handle this?
        if (inst->getRd() == inst->getRd2())
        {
            THROW_ILLEGAL_INST;
        }

        // Write A0 (10) to RS1
        WRITE_INT_REG<XLEN>(state, inst->getRd(), a0_val);
        // Write A1 (11) to RS2
        WRITE_INT_REG<XLEN>(state, inst->getRd2(), a1_val);

        return ++action_it;
    }
} // namespace pegasus
