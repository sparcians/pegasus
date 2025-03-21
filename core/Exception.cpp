#include "core/Exception.hpp"
#include "core/Fetch.hpp"
#include "core/AtlasState.hpp"
#include "include/ActionTags.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "include/CSRFieldIdxs64.hpp"
#include "core/inst_handlers/inst_helpers.hpp"

namespace atlas
{

    Exception::Exception(sparta::TreeNode* exception_node, const ExceptionParameters*) :
        sparta::Unit(exception_node)
    {
        // TODO: Add RV32 exception handling support
        Action exception_action = atlas::Action::createAction<&Exception::handleException_<RV64>>(
            this, "exception", ActionTags::EXCEPTION_TAG);
        exception_action_group_.addAction(exception_action);
    }

    void Exception::onBindTreeEarly_()
    {
        auto core_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
        AtlasState* state = core_tn->getResourceAs<AtlasState>();

        // Connect exception ActionGroup to instruction finish ActionGroup
        exception_action_group_.setNextActionGroup(state->getFinishActionGroup());
    }

    template <typename XLEN> ActionGroup* Exception::handleException_(atlas::AtlasState* state)
    {
        sparta_assert(cause_.isValid(), "Exception cause is not valid!");

        const reg_t trap_handler_address = (READ_CSR_REG<XLEN>(state, MTVEC) & ~(reg_t)1);
        state->setNextPc(trap_handler_address);

        // Get PC that caused the exception
        const reg_t epc_val = state->getPc();
        WRITE_CSR_REG<XLEN>(state, MEPC, epc_val);

        // Get the exception code
        const uint64_t cause_val = static_cast<uint64_t>(cause_.getValue());
        WRITE_CSR_REG<XLEN>(state, MCAUSE, cause_val);

        // Depending on the exception type, get the
        const uint64_t trap_val = determineTrapValue_(cause_.getValue(), state);
        WRITE_CSR_REG<XLEN>(state, MTVAL, trap_val);

        // TODO: For hypervisor, also write tval2 and tinst

        const auto mstatus_mie = READ_CSR_FIELD<XLEN>(state, MSTATUS, "mie");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpie", mstatus_mie);

        const auto mpp_val = static_cast<uint64_t>(state->getPrivMode());
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpp", mpp_val);

        const uint64_t mie_val = 0;
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mie", mie_val);

        const uint64_t mpv_val = 0;
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpv", mpv_val);

        const uint64_t gva_val = 0;
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "gva", gva_val);

        state->setNextPrivMode(PrivMode::MACHINE);

        state->snapshotAndSyncWithCoSim();
        cause_.clearValid();
        return nullptr;
    }

    uint64_t Exception::determineTrapValue_(const TrapCauses & trap_cause, AtlasState* state)
    {
        switch (trap_cause)
        {
            case TrapCauses::MISALIGNED_FETCH:
            case TrapCauses::FETCH_ACCESS:
            case TrapCauses::FETCH_PAGE_FAULT:
                return state->getPc();
                break;
            case TrapCauses::MISALIGNED_LOAD:
            case TrapCauses::MISALIGNED_STORE:
            case TrapCauses::LOAD_ACCESS:
            case TrapCauses::STORE_ACCESS:
            case TrapCauses::LOAD_PAGE_FAULT:
            case TrapCauses::STORE_PAGE_FAULT:
                return state->getCurrentInst()->getTranslationState()->getRequest().getVaddr();
                break;
            case TrapCauses::ILLEGAL_INSTRUCTION:
                return state->getSimState()->current_opcode;
                break;
            case TrapCauses::BREAKPOINT:
            case TrapCauses::USER_ECALL:
            case TrapCauses::SUPERVISOR_ECALL:
            case TrapCauses::MACHINE_ECALL:
            case TrapCauses::SOFTWARE_CHECK_FAULT:
            case TrapCauses::HARDWARE_ERROR_FAULT:
            default:
                return 0;
                break;
        }
    }

} // namespace atlas
