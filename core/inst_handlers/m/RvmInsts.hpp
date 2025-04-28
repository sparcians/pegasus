#pragma once

#include "core/Execute.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvmInsts
    {
      public:
        using base_type = RvmInsts;

        template <typename XLEN> static void getInstHandlers(Execute::InstHandlersMap &);

      private:
        template <typename XLEN> ActionGroup* div_64Handler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* divu_64Handler_(atlas::AtlasState* state);
        ActionGroup* divuw_64Handler_(atlas::AtlasState* state);
        ActionGroup* divw_64Handler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* mul_64Handler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* mulh_64Handler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* mulhsu_64Handler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* mulhu_64Handler_(atlas::AtlasState* state);
        ActionGroup* mulw_64Handler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* rem_64Handler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* remu_64Handler_(atlas::AtlasState* state);
        ActionGroup* remuw_64Handler_(atlas::AtlasState* state);
        ActionGroup* remw_64Handler_(atlas::AtlasState* state);
    };
} // namespace atlas
