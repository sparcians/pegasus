#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"
#include <map>

namespace pegasus
{
    class PegasusState;

    class RvsmrmniInsts
    {
      public:
        using base_type = RvsmrmniInsts!;

        template <typename XLEN, PrivMode PRIV_MODE>
        Action::ItrType xretHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
