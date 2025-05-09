#pragma once

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvvcsInsts
    {
      public:
        using base_type = RvvcsInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, typename VLEN>
        Action* vsetvlHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN, typename VLEN>
        Action* vsetvliHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN, typename VLEN>
        Action* vsetivliHandler_(atlas::AtlasState* state, Action*);
    };
} // namespace atlas
