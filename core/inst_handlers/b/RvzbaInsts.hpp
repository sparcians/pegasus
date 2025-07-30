#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace pegasus
{
    class PegasusState;

    class RvzbaInsts
    {
      public:
        using base_type = RvzbaInsts;

        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:
        template <typename XLEN, uint32_t SHIFT, bool WORD>
        Action::ItrType shxaddHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType slli_uwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
