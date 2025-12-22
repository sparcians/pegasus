#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvhInsts
    {
      public:
        using base_type = RvhInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);

      private:
        // hfence.vvma, hfence.gvma
        template <typename XLEN, bool GUEST>
        Action::ItrType hfenceHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
