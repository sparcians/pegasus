#pragma once

#include "core/Execute.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvzicsrInsts
    {
      public:
        using base_type = RvzicsrInsts;

        template <typename XLEN> static void getInstHandlers(Execute::InstHandlersMap &);

        template <typename XLEN> static void getCsrUpdateActions(Execute::CsrUpdateActionsMap &);

      private:
        enum AccessType
        {
            WRITE,
            READ
        };

        template <AccessType TYPE>
        bool isAccessLegal_(const uint32_t csr_num, const PrivMode priv_mode);

        template <typename XLEN> ActionGroup* csrrcHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrciHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrsHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrsiHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrwHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* csrrwiHandler_(atlas::AtlasState* state);

        // CSR update Actions for executing write side effects
        template <typename XLEN> ActionGroup* fcsrUpdateHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fflagsUpdateHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* frmUpdateHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* misaUpdateHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* sstatusUpdateHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* mstatusUpdateHandler_(atlas::AtlasState* state);
    };
} // namespace atlas
