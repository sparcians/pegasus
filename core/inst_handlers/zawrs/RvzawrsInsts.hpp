#pragma once

#include "core/InstHandlers.hpp"
#include "include/PegasusTypes.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzawrsInsts
    {
      public:
        using base_type = RvzawrsInsts;

        template <typename XLEN>
        static void getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers);

      private:
        template <typename XLEN>
        Action::ItrType wrsntoHandler_(PegasusState* state, Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType wrsstoHandler_(PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
