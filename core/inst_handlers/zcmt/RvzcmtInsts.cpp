#include "core/inst_handlers/zcmt/RvzcmtInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/Trap.hpp"
#include "core/Action.hpp"
#include "include/ActionTags.hpp"
#include "include/IntNums.hpp"
#include "include/gen/CSRHelpers.hpp"
#include "include/gen/CSRBitMasks64.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzcmtInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // Instruction handlers
        inst_handlers.emplace("cm_jt", Action::createAction<&RvzcmtInsts::jt_<XLEN>, RvzcmtInsts>(
                                           nullptr, "cm_jt", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("cm_jalt",
                              Action::createAction<&RvzcmtInsts::jalt_<XLEN>, RvzcmtInsts>(
                                  nullptr, "cm_jalt", ActionTags::EXECUTE_TAG));
    }

    template <typename XLEN>
    void RvzcmtInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // Address calculation
        inst_handlers.emplace(
            "cm_jt", Action::createAction<&RvzcmtInsts::computeAddressHandler_<XLEN>, RvzcmtInsts>(
                         nullptr, "cm_jt", ActionTags::COMPUTE_ADDR_TAG));

        inst_handlers.emplace(
            "cm_jalt",
            Action::createAction<&RvzcmtInsts::computeAddressHandler_<XLEN>, RvzcmtInsts>(
                nullptr, "cm_jalt", ActionTags::COMPUTE_ADDR_TAG));
    }

    template void RvzcmtInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzcmtInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
    template void RvzcmtInsts::getInstComputeAddressHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzcmtInsts::getInstComputeAddressHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN>
    Action::ItrType RvzcmtInsts::computeAddressHandler_(pegasus::PegasusState* state,
                                                        Action::ItrType action_it)
    {
        XLEN mask = (XLEN)JVT_64_bitmasks::BASE;
        XLEN modeMask = (XLEN)JVT_64_bitmasks::MODE;

        if (!isAccessLegal_<AccessType::READ>(JVT, state->getPrivMode()))
        {
            THROW_ILLEGAL_INST;
        }

        if (!checkStateEn0_<XLEN>(state, "JVT"))
        {
            THROW_ILLEGAL_INST;
        }

        // The load address is computed as JVT + XLEN * imm
        const XLEN csr_val = READ_CSR_REG<XLEN>(state, JVT);
        const XLEN base_addr = csr_val & mask;
        const XLEN mode = csr_val & modeMask;

        if (mode == 0)
        {
            const PegasusInstPtr & inst = state->getCurrentInst();
            const XLEN imm = inst->getImmediate();

            const XLEN vaddr = base_addr + sizeof(XLEN) * imm;
            inst->getTranslationState()->makeRequest(vaddr, sizeof(XLEN));
        }
        else
        {
            sparta_assert(false && "Unimplemented JVT mode");
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzcmtInsts::jt_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        // Load jump address
        const XLEN paddr = inst->getTranslationState()->getResult().getPAddr();
        inst->getTranslationState()->popResult();
        const XLEN jump_target = state->readMemory<XLEN>(paddr) & ~0x1;

        // Jump
        state->setNextPc(jump_target);

        // jalt and jt only differ by the linking step
        // A jt is decoded as jalt if the immediate is >= 32
        // Link
        const XLEN imm = inst->getImmediate();
        if (imm >= 32)
        {
            XLEN rd_val = state->getPc() + inst->getOpcodeSize();
            WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzcmtInsts::jalt_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        return jt_<XLEN>(state, action_it);
    }
} // namespace pegasus
