#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"

namespace atlas
{
    class AtlasState;

    class RvvIntegerInsts
    {
      public:
        using base_type = RvvIntegerInsts;

        enum struct OperandMode
        {
            VV, // vector vector
            VX, // vector scaler
            VI, // vector immediate
            WV, // wide   vector
            WX  // wide   scaler
        };

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, OperandMode opMode, template <typename> typename FunctorTemp>
        Action::ItrType viaHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType vwiaHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
