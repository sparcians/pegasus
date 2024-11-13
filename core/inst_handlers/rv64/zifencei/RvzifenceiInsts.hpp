#pragma once

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvzifenceiInsts
    {
      public:
        using base_type = RvzifenceiInsts;

        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        ActionGroup* fence_i_64_handler(atlas::AtlasState* state);
    };
} // namespace atlas
