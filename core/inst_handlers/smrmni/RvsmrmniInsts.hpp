#pragma once

#include "core/InstHandlers.hpp"
#include "core/PegasusCore.hpp"
#include "core/ActionGroup.hpp"

namespace pegasus
{
    class PegasusState;

    class RvsmrmniInsts
    {
      public:
        using base_type = RvsmrmniInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);

      private:
        template <typename XLEN, PrivMode PRIV_MODE>
        Action::ItrType xretHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
