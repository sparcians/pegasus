#pragma once

#include "core/PegasusCore.hpp"

namespace pegasus
{
    class RvCsrAccess
    {
      protected:
        enum AccessType
        {
            WRITE,
            READ
        };

        template <AccessType TYPE>
        bool isAccessLegal_(const uint32_t csr_num, const PrivMode priv_mode);

        template <typename XLEN> bool checkStateEn0_(PegasusState* state, const char* field_name);
    };

    template <RvCsrAccess::AccessType TYPE>
    bool RvCsrAccess::isAccessLegal_(const uint32_t csr_num, const PrivMode priv_mode)
    {
        // From RISC-V spec:
        // The upper 4 bits of the CSR address (csr[11:8]) are used to encode the read and write
        // accessibility of the CSRs according to privilege level. The top two bits (csr[11:10])
        // indicate whether the register is read/write (00,01, or 10) or read-only (11). The next
        // two bits (csr[9:8]) encode the lowest privilege level that can access the CSR.
        const bool is_writable = (csr_num & 0xc00) != 0xc00;
        const PrivMode lowest_priv_level = (PrivMode)((csr_num & 0x300) >> 8);

        return ((TYPE == AccessType::READ) || is_writable) && (priv_mode >= lowest_priv_level);
    }

    template <typename XLEN>
    bool RvCsrAccess::checkStateEn0_(PegasusState* state, const char* field_name)
    {
        using namespace pegasus;

        XLEN allow = true;

        const auto & extensionManager = state->getCore()->getExtensionManager();
        const bool isSstateen = extensionManager.isEnabled("Ssstateen");
        const bool isMstateen = extensionManager.isEnabled("Smstateen");

        switch (state->getPrivMode())
        {
            case PrivMode::USER:
                if (isSstateen && state->getCore()->isPrivilegeModeSupported(PrivMode::SUPERVISOR))
                {
                    allow &= READ_CSR_FIELD<XLEN>(state, SSTATEEN0, field_name);
                }
                [[fallthrough]];

            case PrivMode::SUPERVISOR:
                if (isMstateen && state->getCore()->isPrivilegeModeSupported(PrivMode::HYPERVISOR))
                {
                    allow &= READ_CSR_FIELD<XLEN>(state, HSTATEEN0, field_name);
                }
                [[fallthrough]];

            case PrivMode::HYPERVISOR:
                if (isMstateen)
                {
                    allow &= READ_CSR_FIELD<XLEN>(state, MSTATEEN0, field_name);
                }
                [[fallthrough]];

            case PrivMode::MACHINE:
                break;

            default:
                assert(false && "Reached unimplemented state");
        }

        return allow;
    }
} // namespace pegasus
