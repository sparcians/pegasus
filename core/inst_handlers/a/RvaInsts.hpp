#pragma once

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvaInsts
    {
      public:
        using base_type = RvaInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename T> Action* computeAddressHandler_(AtlasState* state, Action*);

        template <typename RV, typename SIZE, typename OP, bool U = true>
        Action* amoHandler_(atlas::AtlasState* state, Action*);

        Action* lr_d_64Handler_(atlas::AtlasState* state, Action*);
        Action* lr_w_64Handler_(atlas::AtlasState* state, Action*);
        Action* sc_d_64Handler_(atlas::AtlasState* state, Action*);
        Action* sc_w_64Handler_(atlas::AtlasState* state, Action*);
    };
} // namespace atlas
