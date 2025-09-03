#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"

#include <map>

namespace pegasus
{
    class PegasusState;
    class Action;
    class ActionGroup;

    class RvzilsdInsts
    {
      public:
        using base_type = RvzilsdInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> &);
        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:
        // ld and sd
        Action::ItrType computeAddressHandler_(pegasus::PegasusState* state, Action::ItrType);
        Action::ItrType ldHandler_(pegasus::PegasusState* state, Action::ItrType);
        Action::ItrType sdHandler_(pegasus::PegasusState* state, Action::ItrType);
    };
} // namespace pegasus
