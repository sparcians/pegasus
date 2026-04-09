#pragma once

#include "core/InstHandlers.hpp"
#include "core/inst_handlers/v/RvvIntegerInsts.hpp"

namespace pegasus
{
    class PegasusState;

    class RvvZvbcInsts : public RvvIntegerInsts
    {
      public:
        using base_type = RvvZvbcInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);
    };
} // namespace pegasus
