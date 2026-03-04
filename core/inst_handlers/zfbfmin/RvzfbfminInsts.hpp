#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzfbfminInsts
    {
      public:
        using base_type = RvzfbfminInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);
    };
} // namespace pegasus
