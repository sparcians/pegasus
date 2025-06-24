#pragma once

#include "core/Execute.hpp"

namespace atlas
{
    class AtlasState;

    class RvzicondInsts
    {
      public:
        using base_type = RvzicondInsts;

        template <typename XLEN> static void getInstHandlers(Execute::InstHandlersMap &);

      private:
        template <typename XLEN, typename OPERATOR>
        Action::ItrType czeroHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
