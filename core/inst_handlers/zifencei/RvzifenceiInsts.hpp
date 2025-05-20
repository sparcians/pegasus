#pragma once

#include "core/Action.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;
    class ActionGroup;

    class RvzifenceiInsts
    {
      public:
        using base_type = RvzifenceiInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN>
        Action::ItrType fence_iHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
