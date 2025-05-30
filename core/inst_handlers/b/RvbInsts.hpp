#pragma once

#include "include/AtlasTypes.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvbInsts
    {
      public:
        using base_type = RvbInsts;

        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:

        Action::ItrType add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType zextw_handler(atlas::AtlasState* state, Action::ItrType action_it);
        
        //sh1add and sh1add.uw
        template <typename XLEN> ActionGroup* sh1add_handler(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType sh1add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it);
        
        template <typename XLEN> ActionGroup* sh2add_handler(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType sh2add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it);

        template <typename XLEN> ActionGroup* sh3add_handler(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType sh3add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it);

        Action::ItrType sll1uw_handler(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas