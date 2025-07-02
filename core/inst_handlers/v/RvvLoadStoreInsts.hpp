#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvvLoadStoreInsts
    {
      public:
        using base_type = RvvLoadStoreInsts;

        enum struct AddressingMode
        {
            UNIT,
            IDX_UNORDERED,
            STRIDED,
            IDX_ORDERED
        };

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, size_t ElemWidth, AddressingMode addrMode>
        Action::ItrType vlseComputeAddressHandler_(atlas::AtlasState* state,
                                                   Action::ItrType action_it);
        template <typename XLEN, size_t ElemWidth, AddressingMode addrMode>
        Action::ItrType vlseIdxComputeAddressHandler_(atlas::AtlasState* state,
                                                      Action::ItrType action_it);
        template <size_t ElemWidth, bool load>
        Action::ItrType vlseHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <bool load>
        Action::ItrType vlseIdxHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
