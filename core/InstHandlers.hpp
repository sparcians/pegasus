#pragma once

#include "core/ActionGroup.hpp"
#include "include/PegasusTypes.hpp"

#include <map>

namespace pegasus
{
    class InstHandlers
    {
      public:
        using base_type = InstHandlers;

        InstHandlers(const bool enable_syscall_emulation);

        using InstHandlersMap = std::map<std::string, Action>;
        using CsrUpdateActionsMap = std::map<uint32_t, Action>;

        template <typename XLEN> const InstHandlersMap* getInstHandlersMap() const
        {
            static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return &rv64_inst_actions_;
            }
            else
            {
                return &rv32_inst_actions_;
            }
        }

        template <typename XLEN> const InstHandlersMap* getInstComputeAddressHandlersMap() const
        {
            static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return &rv64_inst_compute_address_actions_;
            }
            else
            {
                return &rv32_inst_compute_address_actions_;
            }
        }

        template <typename XLEN> const CsrUpdateActionsMap* getCsrUpdateActionsMap() const
        {
            static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                return &rv64_csr_update_actions_;
            }
            else
            {
                return &rv32_csr_update_actions_;
            }
        }

      private:
        // Instruction handlers
        InstHandlersMap rv64_inst_actions_;
        InstHandlersMap rv32_inst_actions_;

        // Instruction handlers for computing the address of load/store instructions
        InstHandlersMap rv64_inst_compute_address_actions_;
        InstHandlersMap rv32_inst_compute_address_actions_;

        // CSR update Actions for executing write side effects
        CsrUpdateActionsMap rv64_csr_update_actions_;
        CsrUpdateActionsMap rv32_csr_update_actions_;

        // Instruction handler for unsupported instructions
        Action::ItrType unsupportedInstHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it);
    };
} // namespace pegasus
