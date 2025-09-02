#pragma once

#include "core/Action.hpp"

#include <map>
#include <string>
#include <stdint.h>

namespace pegasus
{
    class PegasusState;

    class RvaInstsBase
    {
      public:
        using base_type = RvaInstsBase;

      protected:
        template <typename XLEN>
        Action::ItrType computeAddressHandler_(PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, typename SIZE, typename OP, bool U = true>
        Action::ItrType amoHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, typename SIZE>
        Action::ItrType lr_handler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, typename SIZE>
        Action::ItrType sc_handler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
