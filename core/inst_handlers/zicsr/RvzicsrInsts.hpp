#pragma once

#include "core/Execute.hpp"
#include "core/inst_handlers/zicsr/RvcsrAccess.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzicsrInsts : public RvCsrAccess
    {
      public:
        using base_type = RvzicsrInsts;

        template <typename XLEN> static void getInstHandlers(Execute::InstHandlersMap &);

        template <typename XLEN> static void getCsrUpdateActions(Execute::CsrUpdateActionsMap &);

      private:
        template <typename XLEN>
        Action::ItrType csrrcHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrciHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrsHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrsiHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType csrrwiHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // CSR update Actions for executing write side effects
        template <typename XLEN>
        Action::ItrType fcsrUpdateHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fflagsUpdateHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType frmUpdateHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType sstatusUpdateHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType satpUpdateHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType mstatusUpdateHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType misaUpdateHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
