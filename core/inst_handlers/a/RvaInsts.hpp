#pragma once

#include "core/Action.hpp"

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;

    class RvaInsts
    {
      public:
        using base_type = RvaInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename T>
        Action::ItrType computeAddressHandler_(AtlasState* state, Action::ItrType action_it);

        template <typename RV, typename SIZE, typename OP, bool U = true>
        Action::ItrType amoHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        template <typename XLEN, typename SIZE>
        Action::ItrType lr_handler_(atlas::AtlasState* state, Action::ItrType action_it);

        template <typename XLEN, typename SIZE>
        Action::ItrType sc_handler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
