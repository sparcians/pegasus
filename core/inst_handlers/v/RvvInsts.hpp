#pragma once

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvvInsts
    {
      public:
        using base_type = RvvInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, typename VLEN>
        ActionGroup* vsetvlHandler_(atlas::AtlasState* state);
        template <typename XLEN, typename VLEN>
        ActionGroup* vsetvliHandler_(atlas::AtlasState* state);
        template <typename XLEN, typename VLEN>
        ActionGroup* vsetivliHandler_(atlas::AtlasState* state);
    };
} // namespace atlas
