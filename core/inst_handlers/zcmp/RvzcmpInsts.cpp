#include "core/inst_handlers/zcmp/RvzcmpInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/Trap.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzcmpInsts::getInstHandlers(Execute::InstHandlersMap & inst_handlers)
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

    template void RvzcmpInsts::getInstHandlers<RV32>(Execute::InstHandlersMap &);
    template void RvzcmpInsts::getInstHandlers<RV64>(Execute::InstHandlersMap &);

    template <typename XLEN>
    void pop_(pegasus::PegasusState* state)
    {
        (void)state;
    }

    template <typename XLEN>
    Action::ItrType RvzcmpInsts::pushHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        (void)state;
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
        if (inst->getRs1() == inst->getRs2())
        {
            THROW_ILLEGAL_INST;
        }

        // Write A0 (10) to RS1
        WRITE_INT_REG<XLEN>(state, inst->getRs1(), a0_val);
        // Write A1 (11) to RS2
        WRITE_INT_REG<XLEN>(state, inst->getRs2(), a1_val);

        return ++action_it;
    }
} // namespace pegasus
