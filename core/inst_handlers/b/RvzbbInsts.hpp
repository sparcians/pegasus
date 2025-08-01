#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>
#include <bit>

namespace pegasus
{
    class PegasusState;

    class RvzbbInsts
    {
      public:
        using base_type = RvzbbInsts;

        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:
        template <typename XLEN, typename OP>
        Action::ItrType unaryOpHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename OP>
        Action::ItrType binaryOpHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename OP>
        Action::ItrType immOpHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType orc_bHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
