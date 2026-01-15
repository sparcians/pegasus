#include "core/inst_handlers/zilsd/RvzilsdInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzilsdInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "ld",
            pegasus::Action::createAction<&RvzilsdInsts::computeAddressHandler_, RvzilsdInsts>(
                nullptr, "ld", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "sd",
            pegasus::Action::createAction<&RvzilsdInsts::computeAddressHandler_, RvzilsdInsts>(
                nullptr, "sd", ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvzilsdInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "ld", pegasus::Action::createAction<&RvzilsdInsts::ldHandler_, RvzilsdInsts>(
                      nullptr, "ld", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sd", pegasus::Action::createAction<&RvzilsdInsts::sdHandler_, RvzilsdInsts>(
                      nullptr, "sd", ActionTags::EXECUTE_TAG));
    }

    template void
    RvzilsdInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzilsdInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);

    Action::ItrType RvzilsdInsts::computeAddressHandler_(pegasus::PegasusState* state,
                                                         Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        PegasusTranslationState* translation_state = inst->getTranslationState();

        const RV32 rs1_val = READ_INT_REG<RV32>(state, inst->getRs1());
        const RV32 imm = inst->getImmediate();

        const RV32 vaddr = rs1_val + imm;
        translation_state->makeRequest(vaddr, sizeof(uint64_t));

        return ++action_it;
    }

    Action::ItrType RvzilsdInsts::ldHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        PegasusTranslationState* translation_state = inst->getTranslationState();

        const uint64_t result = state->readMemory<uint64_t>(translation_state->getResult(),
                                                            MemAccessSource::INSTRUCTION);
        translation_state->popResult();

        // Write bits 31:0 to the lower-numbered register
        const RV32 rd1_val = ((1ull << 33) - 1) & result;
        WRITE_INT_REG<RV32>(state, inst->getRd(), rd1_val);

        // Write bits 63:32 to the higher-numbered register
        const RV32 rd2_val = result >> 32;
        WRITE_INT_REG<RV32>(state, inst->getRd2(), rd2_val);

        return ++action_it;
    }

    Action::ItrType RvzilsdInsts::sdHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        PegasusTranslationState* translation_state = inst->getTranslationState();

        const RV32 rs2_val = READ_INT_REG<RV32>(state, inst->getRs2());
        const RV32 rs3_val = READ_INT_REG<RV32>(state, inst->getRs3());

        // lower-numbered register holds bits 31:0
        // higher-numbered register holds bits 63:32
        const uint64_t value = ((uint64_t)rs3_val << 32) | rs2_val;

        state->writeMemory<uint64_t>(translation_state->getResult(), value,
                                     MemAccessSource::INSTRUCTION);
        translation_state->popResult();

        return ++action_it;
    }
} // namespace pegasus
