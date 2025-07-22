#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"
#include "core/inst_handlers/vector_types.hpp"

namespace pegasus
{
    class PegasusState;
    class Action;
    class ActionGroup;

    class RvvPermuteInsts
    {
      public:
        using base_type = RvvPermuteInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, OperandMode opMode>
        Action::ItrType vmvHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, bool isUp>
        Action::ItrType vslideHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, bool isUp>
        Action::ItrType vslide1Handler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, bool is16>
        Action::ItrType vrgatherHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        Action::ItrType vcompressHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <size_t nReg>
        Action::ItrType vmvrHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
