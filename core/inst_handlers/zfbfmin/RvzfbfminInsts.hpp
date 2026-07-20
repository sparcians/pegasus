#pragma once

#include "core/InstHandlers.hpp"
#include "core/inst_handlers/f/RvfInstsBase.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzfbfminInsts : public RvfInstsBase
    {
      public:
        using base_type = RvzfbfminInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);

      private:
        template <typename XLEN>
        Action::ItrType fcvt_bf16_sHandler_(pegasus::PegasusState* state,
                                            Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_s_bf16Handler(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
