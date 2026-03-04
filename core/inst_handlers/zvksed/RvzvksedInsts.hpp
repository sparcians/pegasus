#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzvksedInsts
    {
      public:
        using base_type = RvzvksedInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);
    };
} // namespace pegasus
