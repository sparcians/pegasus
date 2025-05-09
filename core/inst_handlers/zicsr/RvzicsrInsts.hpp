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

        template <typename XLEN> Action* csrrcHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* csrrciHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* csrrsHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* csrrsiHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* csrrwHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* csrrwiHandler_(atlas::AtlasState* state, Action*);

        // CSR update Actions for executing write side effects
        template <typename XLEN> Action* fcsrUpdateHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fflagsUpdateHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* frmUpdateHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* misaUpdateHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* sstatusUpdateHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* mstatusUpdateHandler_(atlas::AtlasState* state, Action*);
    };
} // namespace atlas
