#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"

<<<<<<< HEAD
namespace atlas
{
    class AtlasState;
=======
namespace pegasus
{
    class PegasusState;
>>>>>>> upstream/main
    class Action;
    class ActionGroup;

    class RvvMaskInsts
    {
      public:
        using base_type = RvvMaskInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

        enum struct SetFirstMode
        {
            BEFORE,
            INCLUDING,
            ONLY
        };

      private:
        template <auto func>
<<<<<<< HEAD
        Action::ItrType vmlHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vcpHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vfirstHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);
        template <SetFirstMode sfMode>
        Action::ItrType vsxfHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        Action::ItrType viotaHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType veiHandler_(AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
=======
        Action::ItrType vmlHandler_(pegasus::PegasusState* state_ptr, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vcpHandler_(pegasus::PegasusState* state_ptr, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vfirstHandler_(pegasus::PegasusState* state_ptr, Action::ItrType action_it);
        template <SetFirstMode sfMode>
        Action::ItrType vsxfHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        Action::ItrType viotaHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType veiHandler_(PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
>>>>>>> upstream/main
