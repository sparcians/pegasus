#pragma once

#include <bitset>

#include "core/ActionGroup.hpp"
#include "core/Trap.hpp"
#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/Unit.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{

    class PegasusState;

    /// This class handles exceptions that occur during the fetch-translate-decode-execute cycle,
    /// e.g. for handling illegal instructions.
    class Exception : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "Exception";
        using base_type = Exception;

        class ExceptionParameters : public sparta::ParameterSet
        {
          public:
            ExceptionParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            PARAMETER(uint64_t, unexpected_faults, 0x0, R"(Unexpected faults bitmask.
            Mask to set for the given code:

                 Exception (Code)                 |  Mask
                 ---------------------------------|--------
                 INST_ADDR_MISALIGNED      (0)        0x000001
                 INST_ACCESS               (1)        0x000002
                 ILLEGAL_INST              (2)        0x000004
                 BREAKPOINT                (3)        0x000008
                 LOAD_ADDR_MISALIGNED      (4)        0x000010
                 LOAD_ACCESS               (5)        0x000020
                 STORE_AMO_ADDR_MISALIGNED (6)        0x000040
                 STORE_AMO_ACCESS          (7)        0x000080
                 USER_ECALL                (8)        0x000100
                 SUPERVISOR_ECALL          (9)        0x000200
                 MACHINE_ECALL             (11)       0x000800
                 INST_PAGE_FAULT           (12)       0x001000
                 LOAD_PAGE_FAULT           (13)       0x002000
                 STORE_AMO_PAGE_FAULT      (15)       0x008000
                 SOFTWARE_CHECK            (18)       0x040000
                 HARDWARE_ERROR            (19)       0x080000
)")
            PARAMETER(bool, fail_on_back_to_back_faults, false,
                      "If the simulator detects a back-to-back same fault, stop simulation")
        };

        Exception(sparta::TreeNode* exception_node, const ExceptionParameters* p);

        void setUnhandledException(const FaultCause cause) { fault_cause_ = cause; }

        void setUnhandledException(const InterruptCause cause) { interrupt_cause_ = cause; }

        const sparta::utils::ValidValue<FaultCause> & getUnhandledFault() const
        {
            return fault_cause_;
        }

        const sparta::utils::ValidValue<InterruptCause> & getUnhandledInterrupt() const
        {
            return interrupt_cause_;
        }

        ActionGroup* getActionGroup() { return &exception_action_group_; }

      private:
        void onBindTreeEarly_() override;

        const std::bitset<sizeof(uint64_t) * 8> unexpected_faults_;
        const bool fail_back_to_back_faults_;

        template <typename XLEN>
        Action::ItrType handleException_(pegasus::PegasusState* state, Action::ItrType action_it);

        ActionGroup exception_action_group_{"Exception"};

        Action rv32_exception_action_;
        Action rv64_exception_action_;

        sparta::utils::ValidValue<FaultCause> fault_cause_;
        sparta::utils::ValidValue<InterruptCause> interrupt_cause_;

        uint64_t determineTrapValue_(const FaultCause & trap_cause, PegasusState* state);
        uint64_t determineTrapValue_(const InterruptCause & trap_cause, PegasusState* state);

        uint64_t determineGvaValue_(const FaultCause & trap_cause, const bool virt_mode);
    };

} // namespace pegasus
