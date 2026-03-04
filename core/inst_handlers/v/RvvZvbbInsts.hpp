#pragma once

#include <map>

#include "core/Action.hpp"
#include "core/inst_handlers/v/RvvIntegerInsts.hpp"

namespace pegasus
{
    class PegasusState;

    class RvvZvbbInsts : public RvvIntegerInsts
    {
      public:
        using base_type = RvvZvbbInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        // Integer Widening Shift Left Logical
        template <typename XLEN, OperandMode opMode>
        Action::ItrType viwsllHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };

} // namespace pegasus
