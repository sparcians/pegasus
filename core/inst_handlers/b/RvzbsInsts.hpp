#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace pegasus
{
    class PegasusState;

    class RvzbsInsts
    {
      public:
        using base_type = RvzbsInsts;

        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:
        template <typename XLEN, typename OP>
        Action::ItrType binaryOpHandler(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename OP>
        Action::ItrType immOpHandler(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
