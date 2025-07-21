#pragma once

#include "core/Execute.hpp"

<<<<<<< HEAD
namespace atlas
{
    class AtlasState;
=======
namespace pegasus
{
    class PegasusState;
>>>>>>> upstream/main

    class RvzicondInsts
    {
      public:
        using base_type = RvzicondInsts;

        template <typename XLEN> static void getInstHandlers(Execute::InstHandlersMap &);

      private:
        template <typename XLEN, typename OPERATOR>
<<<<<<< HEAD
        Action::ItrType czeroHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
=======
        Action::ItrType czeroHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
>>>>>>> upstream/main
