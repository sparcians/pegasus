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
        sparta_assert(fault_cause_.isValid() || interrupt_cause_.isValid(), "Exception cause is not valid!");

        const bool is_interrupt = interrupt_cause_.isValid();
        const uint64_t cause_val = is_interrupt ? static_cast<uint64_t>(interrupt_cause_.getValue()) : static_cast<uint64_t>(fault_cause_.getValue());

        // Determine which privilege mode to handle the trap in
        const uint32_t trap_deleg_csr = is_interrupt ? MEDELEG : MIDELEG;
        const XLEN trap_deleg = READ_CSR_REG<XLEN>(state, trap_deleg_csr);
        const PrivMode priv_mode = ((1(XLEN) << (cause_val)) & trap_deleg_val) ? PrivMode::SUPERVISOR : PrivMode::MACHINE;
        state->setNextPrivMode(priv_mode);

        // Set next PC
        const uint64_t tvec_csr = (priv_mode == PrivMode::SUPERVISOR) ? STVEC : MTVEC;
        const reg_t trap_handler_address = (READ_CSR_REG<XLEN>(state, tvec_csr) & ~(reg_t)1);
        state->setNextPc(trap_handler_address);

        // Get PC that caused the exception
        const reg_t epc_val = state->getPc();
        const uint32_t epc_csr =  (priv_mode == PrivMode::SUPERVISOR) ? SEPC : MEPC;
        WRITE_CSR_REG<XLEN>(state, epc_csr, epc_val);

        // Get the exception code
        const uint32_t cause_csr = (priv_mode == PrivMode::SUPERVISOR) ? SCAUSE : MCAUSE;
        WRITE_CSR_REG<XLEN>(state, cause_csr, cause_val);

        // Depending on the exception type, get the
        const uint32_t tval_csr = (priv_mode == PrivMode::SUPERVISOR) ? STVAL : MTVAL;
        const uint64_t trap_val = is_interrupt ? determineTrapValue_(interrupt_cause_.getValue(), state) : determineTrapValue_(fault_cause_.getValue(), state);
        WRITE_CSR_REG<XLEN>(state, tval_csr, trap_val);

        // Update MSTATUS
        if (priv_mode == PrivMode::SUPERVISOR)
        {
            const auto mstatus_sie = READ_CSR_FIELD<XLEN>(state, MSTATUS, "sie");
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "spie", mstatus_sie);

            const auto spp_val = static_cast<uint64_t>(state->getPrivMode());
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "spp", mpp_val);

            const uint64_t sie_val = 0;
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "sie", mie_val);

        }
        else 
        {
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
        }

        state->snapshotAndSyncWithCoSim();
        cause_.clearValid();
        return nullptr;
    }

    uint64_t Exception::determineTrapValue_(const FaultCause & trap_cause, AtlasState* state)
    {
        switch (trap_cause)
        {
            case FaultCause::MISALIGNED_FETCH:
            case FaultCause::FETCH_ACCESS:
            case FaultCause::FETCH_PAGE_FAULT:
                return state->getPc();
                break;
            case FaultCause::MISALIGNED_LOAD:
            case FaultCause::MISALIGNED_STORE:
            case FaultCause::LOAD_ACCESS:
            case FaultCause::STORE_ACCESS:
            case FaultCause::LOAD_PAGE_FAULT:
            case FaultCause::STORE_PAGE_FAULT:
                return state->getTranslationState()->getTranslationRequest().getVaddr();
                break;
            case FaultCause::ILLEGAL_INSTRUCTION:
                return state->getSimState()->current_opcode;
                break;
            case FaultCause::BREAKPOINT:
            case FaultCause::USER_ECALL:
            case FaultCause::SUPERVISOR_ECALL:
            case FaultCause::MACHINE_ECALL:
            case FaultCause::SOFTWARE_CHECK_FAULT:
            case FaultCause::HARDWARE_ERROR_FAULT:
            default:
                return 0;
                break;
        }
    }

} // namespace atlas
