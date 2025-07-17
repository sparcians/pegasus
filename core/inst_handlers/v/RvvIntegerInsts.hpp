#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"

namespace pegasus
{
    class PegasusState;

    class RvvIntegerInsts
    {
      public:
        using base_type = RvvIntegerInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <template <typename> typename OP>
        Action::ItrType viavvHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, template <typename> typename OP>
        Action::ItrType viavxHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <template <typename> typename OP>
        Action::ItrType viaviHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
