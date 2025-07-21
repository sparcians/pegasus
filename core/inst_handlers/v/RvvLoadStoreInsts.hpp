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
<<<<<<< HEAD
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
=======
        template <typename XLEN, size_t elemWidth, AddressingMode addrMode>
        Action::ItrType vlseComputeAddressHandler_(pegasus::PegasusState* state,
                                                   Action::ItrType action_it);
        template <typename XLEN, size_t elemWidth, AddressingMode addrMode>
        Action::ItrType vlseIdxComputeAddressHandler_(pegasus::PegasusState* state,
                                                      Action::ItrType action_it);
        template <size_t elemWidth, bool load>
        Action::ItrType vlseHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <bool load>
        Action::ItrType vlseIdxHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
>>>>>>> upstream/main
