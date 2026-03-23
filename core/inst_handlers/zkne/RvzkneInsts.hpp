#pragma once
#include "core/Action.hpp"
#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzkneInsts
    {
        public:
            using base_type = RvzkneInsts;
            
            template <typename XLEN>
            static void getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers);

        private:

            template <typename XLEN, typename OPERATOR>
            Action::ItrType aesHandler_(PegasusState* state, Action::ItrType action_it);
    };
}