#pragma once

#include "core/InstHandlers.hpp"
#include "include/PegasusTypes.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzacasInsts
    {
      public:
        using base_type = RvzacasInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);

        template <typename XLEN>
        static void getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers);

      private:
        template <typename XLEN, typename AccessType>
        Action::ItrType computeAddressHandler_(PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, typename AccessType>
        Action::ItrType amocasHandler_(PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
