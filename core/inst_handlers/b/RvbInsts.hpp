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

        template <typename XLEN> 
        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:

        ActionGroup* add_uw_handler(atlas::AtlasState* state);
        ActionGroup* zextw_handler(atlas::AtlasState* state);
        
        //sh1add and sh1add.uw
        template <typename XLEN> ActionGroup* sh1add_handler(atlas::AtlasState* state);
        ActionGroup* sh1add_uw_handler(atlas::AtlasState* state);
        
        template <typename XLEN> ActionGroup* sh2add_handler(atlas::AtlasState* state);
        ActionGroup* sh2add_uw_handler(atlas::AtlasState* state);

        template <typename XLEN> ActionGroup* sh3add_handler(atlas::AtlasState* state);
        ActionGroup* sh3add_uw_handler(atlas::AtlasState* state);

        ActionGroup* sll1uw_handler(atlas::AtlasState* state);
    };
} // namespace atlas