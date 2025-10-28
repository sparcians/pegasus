#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzcmpInsts
    {
      public:
        using base_type = RvzcmpInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);

      private:
        // Pop implementation
        template <typename XLEN> void pop_(pegasus::PegasusState* state);

        // cm.push
        template <typename XLEN>
        Action::ItrType pushHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // cm.pop
        template <typename XLEN>
        Action::ItrType popHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // cm.popret
        template <typename XLEN>
        Action::ItrType popretHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // cm.popretz
        template <typename XLEN>
        Action::ItrType popretzHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // cm.mva01s
        template <typename XLEN>
        Action::ItrType mva01sHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // cm.mvsa01
        template <typename XLEN>
        Action::ItrType mvsa01Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
