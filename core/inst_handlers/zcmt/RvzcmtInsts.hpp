#pragma once

#include "core/Execute.hpp"
#include "core/inst_handlers/zicsr/RvcsrAccess.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzcmtInsts : public RvCsrAccess
    {
      public:
        using base_type = RvzcmtInsts;

        template <typename XLEN> static void getInstHandlers(Execute::InstHandlersMap &);
        template <typename XLEN>
        static void getInstComputeAddressHandlers(Execute::InstHandlersMap &);

      private:
        // compute address for loads and stores
        template <typename XLEN>
        Action::ItrType computeAddressHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it);

        // cm.jt
        template <typename XLEN>
        Action::ItrType jt_(pegasus::PegasusState* state, Action::ItrType action_it);

        // cm.jalt
        template <typename XLEN>
        Action::ItrType jalt_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
