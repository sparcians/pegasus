#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"
#include "core/inst_handlers/vector_types.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvvPermuteInsts
    {
      public:
        using base_type = RvvPermuteInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN>
        Action::ItrType vmvxsHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vmvsxHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);
    };
} // namespace atlas
