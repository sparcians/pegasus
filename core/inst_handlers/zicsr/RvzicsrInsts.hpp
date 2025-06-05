#pragma once

#include "core/Execute.hpp"

namespace atlas
{
    class AtlasState;

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

        template <typename XLEN>
        Action::ItrType csrrcHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrciHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrsHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrsiHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrwHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrwiHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // CSR update Actions for executing write side effects
        template <typename XLEN>
        Action::ItrType fcsrUpdateHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fflagsUpdateHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType frmUpdateHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType sstatusUpdateHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType satpUpdateHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType mstatusUpdateHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType misaUpdateHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
