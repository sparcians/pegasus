#pragma once

#include "core/Action.hpp"

#include <map>
#include <string>

namespace pegasus
{
    class PegasusState;
    class ActionGroup;

    class RvzihintpauseInsts
    {
      public:
        using base_type = RvzihintpauseInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN>
        Action::ItrType pauseHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
