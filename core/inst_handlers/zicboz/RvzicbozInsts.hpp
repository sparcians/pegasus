#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzicbozInsts
    {
      public:
        using base_type = RvzicbozInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);
    };
} // namespace pegasus
