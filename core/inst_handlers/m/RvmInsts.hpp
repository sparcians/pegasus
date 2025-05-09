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
        template <typename XLEN> Action* div_64Handler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* divu_64Handler_(atlas::AtlasState* state, Action*);
        Action* divuw_64Handler_(atlas::AtlasState* state, Action*);
        Action* divw_64Handler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* mul_64Handler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* mulh_64Handler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* mulhsu_64Handler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* mulhu_64Handler_(atlas::AtlasState* state, Action*);
        Action* mulw_64Handler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* rem_64Handler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* remu_64Handler_(atlas::AtlasState* state, Action*);
        Action* remuw_64Handler_(atlas::AtlasState* state, Action*);
        Action* remw_64Handler_(atlas::AtlasState* state, Action*);
    };
} // namespace atlas
