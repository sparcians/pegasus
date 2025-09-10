#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"
#include "core/inst_handlers/vector_types.hpp"

namespace pegasus
{
    class PegasusState;

    class RvvFixedPointInsts
    {
      public:
        using base_type = RvvFixedPointInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType vxBinaryHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType vxNClipHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
