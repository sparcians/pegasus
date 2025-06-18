#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvvmInsts
    {
      public:
        using base_type = RvvmInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <auto func>
        Action::ItrType vmlHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vcpHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vfirstHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);
    };
} // namespace atlas
