#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"

extern "C"
{
#include "source/RISCV/specialize.h"
#include "source/include/internals.h"
}

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvvFloatInsts
    {
      public:
        using base_type = RvvFloatInsts;

        enum struct OperandMode
        {
            VV, // vector vector
            VF  // vector float
        };

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, OperandMode opMode>
        Action::ItrType vfaddHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode>
        Action::ItrType vfmaccHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
