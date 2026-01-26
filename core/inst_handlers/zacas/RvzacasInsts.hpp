#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzacasInsts
    {
      public:
        using base_type = RvzacasInsts;

        enum struct AccessSize : size_t
        {
            Word = 4,
            Double = 8,
            Quadword = 16
        };

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);

        template <typename XLEN>
        static void getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers);

      private:
        template <typename XLEN, AccessSize accessSize>
        Action::ItrType computeAddressHandler_(PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, AccessSize accessSize>
        Action::ItrType amocasHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
