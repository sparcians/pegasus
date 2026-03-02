#include "core/Exception.hpp"
#include "core/Fetch.hpp"
#include "core/PegasusCore.hpp"
#include "include/ActionTags.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "include/gen/CSRFieldIdxs64.hpp"
#include "core/inst_handlers/inst_helpers.hpp"

namespace pegasus
{

    Exception::Exception(sparta::TreeNode* exception_node,
                         const ExceptionParameters* except_params) :
        sparta::Unit(exception_node),
        unexpected_faults_(except_params->unexpected_faults),
        fail_back_to_back_faults_(except_params->fail_on_back_to_back_faults)
    {
        rv32_exception_action_ = pegasus::Action::createAction<&Exception::handleException_<RV32>>(
            this, "exception", ActionTags::EXCEPTION_TAG);
        rv64_exception_action_ = pegasus::Action::createAction<&Exception::handleException_<RV64>>(
            this, "exception", ActionTags::EXCEPTION_TAG);
    }

    void Exception::onBindTreeEarly_()
    {
        auto hart_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
        PegasusState* state = hart_tn->getResourceAs<PegasusState>();

        const auto xlen = state->getXlen();
        if (xlen == 64)
        {
            exception_action_group_.addAction(rv64_exception_action_);
        }
        else
        {
            exception_action_group_.addAction(rv32_exception_action_);
        }

        // Connect exception ActionGroup to instruction finish ActionGroup
        exception_action_group_.setNextActionGroup(state->getFinishActionGroup());
    }

    template <typename XLEN>
    Action::ItrType Exception::handleException_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        sparta_assert(fault_cause_.isValid() || interrupt_cause_.isValid(),
                      "Exception cause is not valid!");
        sparta_assert(fault_cause_.isValid() != interrupt_cause_.isValid(),
                      "Fault and interrupt cause cannot both be valid!");

        const bool is_interrupt = interrupt_cause_.isValid();
        const uint64_t excp_code = is_interrupt ? static_cast<XLEN>(interrupt_cause_.getValue())
                                                : static_cast<XLEN>(fault_cause_.getValue());

        DLOG("Exception code: " << excp_code);

        // When a trap occurs in HS-mode or U-mode, it goes to M-mode, unless
        // delegated by medeleg or mideleg, in which case it goes to HS-mode.
        // When a trap occurs in VS-mode or VU-mode, it goes to M-mode, unless
        // delegated by medeleg or mideleg, in which case it goes to HS-mode,
        // unless further delegated by hedeleg or hideleg, in which case it goes
        // to VS-mode.
        const uint32_t trap_deleg_csr = is_interrupt ? MIDELEG : MEDELEG;
        const XLEN trap_deleg_val = READ_CSR_REG<XLEN>(state, trap_deleg_csr);
        const PrivMode priv_mode =
            ((1ull << (excp_code)) & trap_deleg_val) ? PrivMode::SUPERVISOR : PrivMode::MACHINE;
        // Traps taken into M-mode or HS-mode set the virtualization mode to 0
        bool virt_mode = false;
        if (state->getVirtualMode() && (priv_mode == PrivMode::SUPERVISOR))
        {
            const uint32_t hyp_trap_deleg_csr = is_interrupt ? HIDELEG : HEDELEG;
            const XLEN hyp_trap_deleg_val = READ_CSR_REG<XLEN>(state, hyp_trap_deleg_csr);
            if ((1ull << (excp_code)) & hyp_trap_deleg_val)
            {
                virt_mode = true;
            }
        }

        if (false == is_interrupt)
        {
            sparta_assert(0 == unexpected_faults_.test(static_cast<XLEN>(fault_cause_.getValue())),
                          "Unexpected fault: " << fault_cause_.getValue());

            if (SPARTA_EXPECT_FALSE(fail_back_to_back_faults_))
            {
                // Check to see if we're throwing the same exception back to
                // back (like illop to illop).  We can be in an infinite loop
                if (SPARTA_EXPECT_FALSE(state->getCurrentException() == excp_code))
                {
                    const auto current_epc = READ_CSR_REG<XLEN>(
                        state, (priv_mode == PrivMode::SUPERVISOR ? SEPC : MEPC));
                    sparta_assert(current_epc != state->getPc(),
                                  "Currently handling fault '"
                                      << static_cast<FaultCause>(state->getCurrentException())
                                      << "' when same fault happened: '"
                                      << static_cast<FaultCause>(excp_code)
                                      << "' at PC: " << HEX16(state->getPc()));
                }
            }
        }

        // Next PC
        XLEN trap_handler_address = std::numeric_limits<XLEN>::max();

        // Current virtualization mode
        const bool prev_virt_mode = state->getVirtualMode();
        // PC that caused the exception
        const XLEN epc_val = state->getPc();
        // Get the exception code, handles interrupts and virtual traps
        const XLEN interrupt_bit = 1 << ((sizeof(XLEN) * 4) - 1);
        const XLEN cause_val = is_interrupt ? (excp_code & interrupt_bit) : excp_code;
        // Depending on the exception type, get the trap value
        const uint64_t trap_val = is_interrupt
                                      ? determineTrapValue_(interrupt_cause_.getValue(), state)
                                      : determineTrapValue_(fault_cause_.getValue(), state);
        // Values for updating VSSTATUS/SSTATUS
        const auto mstatus_sie = READ_CSR_FIELD<XLEN>(state, MSTATUS, "sie");
        const auto xpp_val = static_cast<XLEN>(state->getPrivMode());
        const uint64_t sie_val = 0;

        // VS-mode
        if (virt_mode && (priv_mode == PrivMode::SUPERVISOR))
        {
            const TrapVectorMode vstvec_mode =
                (TrapVectorMode)READ_CSR_FIELD<XLEN>(state, VSTVEC, "mode");
            const XLEN vstvec_base = READ_CSR_FIELD<XLEN>(state, VSTVEC, "base") << 2;
            if (vstvec_mode == TrapVectorMode::DIRECT)
            {
                trap_handler_address = vstvec_base;
            }
            if (vstvec_mode == TrapVectorMode::VECTORED)
            {
                trap_handler_address = vstvec_base + 4 * cause_val * (XLEN)is_interrupt;
            }

            WRITE_CSR_REG<XLEN>(state, VSEPC, epc_val);
            WRITE_CSR_REG<XLEN>(state, VSCAUSE, cause_val);
            WRITE_CSR_REG<XLEN>(state, VSTVAL, trap_val);

            // Update VSSTATUS
            WRITE_CSR_FIELD<XLEN>(state, VSSTATUS, "spie", mstatus_sie);
            WRITE_CSR_FIELD<XLEN>(state, VSSTATUS, "spp", xpp_val);
            WRITE_CSR_FIELD<XLEN>(state, VSSTATUS, "sie", sie_val);
        }
        // HS-mode
        else if (priv_mode == PrivMode::SUPERVISOR)
        {
            const TrapVectorMode stvec_mode =
                (TrapVectorMode)READ_CSR_FIELD<XLEN>(state, STVEC, "mode");
            const XLEN stvec_base = READ_CSR_FIELD<XLEN>(state, STVEC, "base") << 2;
            if (stvec_mode == TrapVectorMode::DIRECT)
            {
                trap_handler_address = stvec_base;
            }
            if (stvec_mode == TrapVectorMode::VECTORED)
            {
                trap_handler_address = stvec_base + 4 * cause_val * (XLEN)is_interrupt;
            }

            WRITE_CSR_REG<XLEN>(state, SEPC, epc_val);
            WRITE_CSR_REG<XLEN>(state, SCAUSE, cause_val);
            WRITE_CSR_REG<XLEN>(state, STVAL, trap_val);

            // Update SSTATUS
            WRITE_CSR_FIELD<XLEN>(state, SSTATUS, "spie", mstatus_sie);
            WRITE_CSR_FIELD<XLEN>(state, SSTATUS, "spp", xpp_val);
            WRITE_CSR_FIELD<XLEN>(state, SSTATUS, "sie", sie_val);

            // Update HSTATUS
            if (state->getCore()->hasHypervisor())
            {
                const uint64_t spv_val = prev_virt_mode;
                WRITE_CSR_FIELD<XLEN>(state, HSTATUS, "spv", spv_val);

                if (prev_virt_mode)
                {
                    WRITE_CSR_FIELD<XLEN>(state, HSTATUS, "spvp", xpp_val);
                }

                const uint64_t gva_val =
                    !is_interrupt ? determineGvaValue_(fault_cause_.getValue(), prev_virt_mode) : 0;
                WRITE_CSR_FIELD<XLEN>(state, HSTATUS, "gva", gva_val);

                // TODO: Guest physical address that faulted, shifted right by 2 bits
                const uint64_t htval_val = 0;
                WRITE_CSR_REG<XLEN>(state, HTVAL, htval_val);

                const uint64_t htinst_val = state->getSimState()->current_opcode;
                WRITE_CSR_REG<XLEN>(state, HTINST, htinst_val);
            }
        }
        // M-mode
        else if (priv_mode == PrivMode::MACHINE)
        {
            const TrapVectorMode mtvec_mode =
                (TrapVectorMode)READ_CSR_FIELD<XLEN>(state, MTVEC, "mode");
            const XLEN mtvec_base = READ_CSR_FIELD<XLEN>(state, MTVEC, "base") << 2;
            if (mtvec_mode == TrapVectorMode::DIRECT)
            {
                trap_handler_address = mtvec_base;
            }
            if (mtvec_mode == TrapVectorMode::VECTORED)
            {
                trap_handler_address = mtvec_base + 4 * cause_val * (XLEN)is_interrupt;
            }

            WRITE_CSR_REG<XLEN>(state, MEPC, epc_val);
            WRITE_CSR_REG<XLEN>(state, MCAUSE, cause_val);
            WRITE_CSR_REG<XLEN>(state, MTVAL, trap_val);

            // Update MSTATUS
            const auto mstatus_mie = READ_CSR_FIELD<XLEN>(state, MSTATUS, "mie");
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpie", mstatus_mie);
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpp", xpp_val);
            const uint64_t mie_val = 0;
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mie", mie_val);

            if (state->getCore()->hasHypervisor())
            {
                // Previous virtualization mode
                const uint64_t mpv_val = prev_virt_mode;

                // Set when a guest virtual address is written to mtval
                const uint64_t gva_val =
                    !is_interrupt ? determineGvaValue_(fault_cause_.getValue(), prev_virt_mode) : 0;

                if constexpr (std::is_same_v<XLEN, RV64>)
                {
                    WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mpv", mpv_val);
                    WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "gva", gva_val);
                }
                else
                {
                    WRITE_CSR_FIELD<XLEN>(state, MSTATUSH, "mpv", mpv_val);
                    WRITE_CSR_FIELD<XLEN>(state, MSTATUSH, "gva", gva_val);
                }
            }
        }
        else
        {
            sparta_assert(false, "Failed to take trap to privilege mode: " << priv_mode);
        }

        state->setNextPc(trap_handler_address);
        state->setPrivMode(priv_mode, virt_mode);
        state->updateTranslationMode<XLEN>(translate_types::TranslationStage::SUPERVISOR);
        state->updateTranslationMode<XLEN>(translate_types::TranslationStage::VIRTUAL_SUPERVISOR);
        state->updateTranslationMode<XLEN>(translate_types::TranslationStage::GUEST);

        fault_cause_.clearValid();
        interrupt_cause_.clearValid();

        state->setCurrentException(excp_code);

        return ++action_it;
    }

    uint64_t Exception::determineTrapValue_(const FaultCause & cause, PegasusState* state)
    {
        // stval must be written with the faulting virtual address for load, store, and instruction
        // page-fault, access-fault, and misaligned exceptions, and for breakpoint exceptions other
        // than those caused by execution of the ebreak or c.ebreak instructions.
        // For illegal-instruction exceptions, stval must be written with the faulting instruction.
        switch (cause)
        {
            case FaultCause::INST_ADDR_MISALIGNED:
            case FaultCause::INST_ACCESS:
            case FaultCause::INST_PAGE_FAULT:
            case FaultCause::INST_GUEST_PAGE_FAULT:
                {
                    const auto request = state->getFetchTranslationState()->getRequest();
                    return (request.getVAddr() + request.getMisalignedBytes());
                }
            case FaultCause::LOAD_ADDR_MISALIGNED:
            case FaultCause::LOAD_ACCESS:
            case FaultCause::STORE_AMO_ADDR_MISALIGNED:
            case FaultCause::STORE_AMO_ACCESS:
            case FaultCause::LOAD_PAGE_FAULT:
            case FaultCause::STORE_AMO_PAGE_FAULT:
            case FaultCause::LOAD_GUEST_PAGE_FAULT:
            case FaultCause::STORE_AMO_GUEST_PAGE_FAULT:
                {
                    const auto vaddr_val =
                        state->getCurrentInst()->getTranslationState()->getRequest().getVAddr();
                    return vaddr_val;
                }
            case FaultCause::ILLEGAL_INST:
            case FaultCause::ILLEGAL_VIRTUAL_INST:
                return state->getSimState()->current_opcode;
            case FaultCause::BREAKPOINT:
            case FaultCause::USER_ECALL:
            case FaultCause::SUPERVISOR_ECALL:
            case FaultCause::VIRTUAL_SUPERVISOR_ECALL:
            case FaultCause::MACHINE_ECALL:
            case FaultCause::SOFTWARE_CHECK:
            case FaultCause::HARDWARE_ERROR:
                return 0;
        }
        return 0;
    }

    uint64_t Exception::determineTrapValue_(const InterruptCause & cause, PegasusState*)
    {
        switch (cause)
        {
            case InterruptCause::SUPERVISOR_SOFTWARE:
            case InterruptCause::VIRTUAL_SUPERVISOR_SOFTWARE:
            case InterruptCause::MACHINE_SOFTWARE:
            case InterruptCause::SUPERVISOR_TIMER:
            case InterruptCause::VIRTUAL_SUPERVISOR_TIMER:
            case InterruptCause::MACHINE_TIMER:
            case InterruptCause::SUPERVISOR_EXTERNAL:
            case InterruptCause::VIRTUAL_SUPERVISOR_EXTERNAL:
            case InterruptCause::MACHINE_EXTERNAL:
            case InterruptCause::SUPERVISOR_GUEST_EXTERNAL:
            case InterruptCause::COUNTER_OVERFLOW:
                return 0;
        }
        return 0;
    }

    uint64_t Exception::determineGvaValue_(const FaultCause & cause, const bool virt_mode)
    {
        if (virt_mode)
        {
            switch (cause)
            {
                case FaultCause::INST_ADDR_MISALIGNED:
                case FaultCause::INST_ACCESS:
                case FaultCause::INST_PAGE_FAULT:
                case FaultCause::LOAD_ADDR_MISALIGNED:
                case FaultCause::LOAD_ACCESS:
                case FaultCause::STORE_AMO_ADDR_MISALIGNED:
                case FaultCause::STORE_AMO_ACCESS:
                case FaultCause::LOAD_PAGE_FAULT:
                case FaultCause::STORE_AMO_PAGE_FAULT:
                case FaultCause::INST_GUEST_PAGE_FAULT:
                case FaultCause::LOAD_GUEST_PAGE_FAULT:
                case FaultCause::STORE_AMO_GUEST_PAGE_FAULT:
                    return 1;
                case FaultCause::ILLEGAL_INST:
                case FaultCause::ILLEGAL_VIRTUAL_INST:
                case FaultCause::BREAKPOINT:
                case FaultCause::USER_ECALL:
                case FaultCause::SUPERVISOR_ECALL:
                case FaultCause::VIRTUAL_SUPERVISOR_ECALL:
                case FaultCause::MACHINE_ECALL:
                case FaultCause::SOFTWARE_CHECK:
                case FaultCause::HARDWARE_ERROR:
                    return 0;
            }
        }
        return 0;
    }

    std::ostream & operator<<(std::ostream & os, const FaultCause & cause)
    {
        switch (cause)
        {
            case FaultCause::INST_ADDR_MISALIGNED:
                os << "INST_ADDR_MISALIGNED";
                break;
            case FaultCause::INST_ACCESS:
                os << "INST_ACCESS";
                break;
            case FaultCause::INST_PAGE_FAULT:
                os << "INST_PAGE_FAULT";
                break;
            case FaultCause::LOAD_ADDR_MISALIGNED:
                os << "LOAD_ADDR_MISALIGNED";
                break;
            case FaultCause::LOAD_ACCESS:
                os << "LOAD_ACCESS";
                break;
            case FaultCause::STORE_AMO_ADDR_MISALIGNED:
                os << "STORE_AMO_ADDR_MISALIGNED";
                break;
            case FaultCause::STORE_AMO_ACCESS:
                os << "STORE_AMO_ACCESS";
                break;
            case FaultCause::LOAD_PAGE_FAULT:
                os << "LOAD_PAGE_FAULT";
                break;
            case FaultCause::STORE_AMO_PAGE_FAULT:
                os << "STORE_AMO_PAGE_FAULT";
                break;
            case FaultCause::ILLEGAL_INST:
                os << "ILLEGAL_INST";
                break;
            case FaultCause::BREAKPOINT:
                os << "BREAKPOINT";
                break;
            case FaultCause::USER_ECALL:
                os << "USER_ECALL";
                break;
            case FaultCause::SUPERVISOR_ECALL:
                os << "SUPERVISOR_ECALL";
                break;
            case FaultCause::VIRTUAL_SUPERVISOR_ECALL:
                os << "VIRTUAL_SUPERVISOR_ECALL";
                break;
            case FaultCause::MACHINE_ECALL:
                os << "MACHINE_ECALL";
                break;
            case FaultCause::SOFTWARE_CHECK:
                os << "SOFTWARE_CHECK";
                break;
            case FaultCause::HARDWARE_ERROR:
                os << "HARDWARE_ERROR";
                break;
            case FaultCause::INST_GUEST_PAGE_FAULT:
                os << "INST_GUEST_PAGE_FAULT";
                break;
            case FaultCause::LOAD_GUEST_PAGE_FAULT:
                os << "LOAD_GUEST_PAGE_FAULT";
                break;
            case FaultCause::ILLEGAL_VIRTUAL_INST:
                os << "ILLEGAL_VIRTUAL_INST";
                break;
            case FaultCause::STORE_AMO_GUEST_PAGE_FAULT:
                os << "STORE_AMO_GUEST_PAGE_FAULT";
                break;
        }
        return os;
    }
} // namespace pegasus
