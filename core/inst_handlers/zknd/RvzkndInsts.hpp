#pragma once
#include "core/Action.hpp"

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzkndInsts
    {
      public:
        using base_type = RvzkndInsts;

        template <typename XLEN>
        static void getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers);

      private:
        template <typename XLEN, typename OPERATOR> // For I type instruction handling
        Action::ItrType aesIHandler_(PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, typename OPERATOR> // for R type instruction handling
        Action::ItrType aesRHandler_(PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, typename OPERATOR>
        Action::ItrType aesKsiOpHandler_(PegasusState* state, Action::ItrType action_it);
    };

} // namespace pegasus