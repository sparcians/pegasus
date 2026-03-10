#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzicbomInsts
    {
      public:
        using base_type = RvzicbomInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);
    };
} // namespace pegasus
