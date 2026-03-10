#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzvkshInsts
    {
      public:
        using base_type = RvzvkshInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);
    };
} // namespace pegasus
