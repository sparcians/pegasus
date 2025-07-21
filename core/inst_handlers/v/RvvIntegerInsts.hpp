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
#include "core/inst_handlers/vector_types.hpp"

namespace pegasus
{
    class PegasusState;
>>>>>>> upstream/main

    class RvvIntegerInsts
    {
      public:
        using base_type = RvvIntegerInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
<<<<<<< HEAD
        template <template <typename> typename OP>
        Action::ItrType viavvHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN, template <typename> typename OP>
        Action::ItrType viavxHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <template <typename> typename OP>
        Action::ItrType viaviHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
=======
        // Ingeter Arithmetic / Bitwise Logical
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType viablHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Result for Integer Add-with-carry Subtract-with-borrow
        template <typename XLEN, OperandMode opMode, bool hasMaskOp,
                  template <typename> typename FunctorTemp>
        Action::ItrType viacsbHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Carry/borrow for Integer Add-with-carry Subtract-with-borrow
        template <typename XLEN, OperandMode opMode, bool hasMaskOp, auto detectFuc>
        Action::ItrType vmiacsbHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Ingeter Compare
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType vmicHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
>>>>>>> upstream/main
