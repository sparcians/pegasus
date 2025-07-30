#pragma once

#include <map>
#include <string>
#include "core/Action.hpp"

namespace pegasus
{

    class PegasusState;

    class RvvReductionInsts
    {
      public:
        using base_type = RvvReductionInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <template <typename> typename OP>
        Action::ItrType vredsumvsHandler_(PegasusState* state, Action::ItrType action_it);
    };

} // namespace pegasus
