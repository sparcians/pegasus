#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"

namespace atlas
{
    class AtlasState;

    class RvvIntegerInsts
    {
      public:
        using base_type = RvvIntegerInsts;

        struct OperandMode
        {
            enum struct Mode
            {
                V, // vector
                X, // scaler
                I, // immediate
                W  // wide
            };

            Mode dst, src1, src2;

            constexpr OperandMode(Mode dst, Mode src2, Mode src1) : dst(dst), src1(src1), src2(src2)
            {
            }
        };

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        // Ingeter Arithmetic / Bitwise Logical
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType viablHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // Result for Integer Add-with-carry Subtract-with-borrow
        template <typename XLEN, OperandMode opMode, bool hasMaskOp,
                  template <typename> typename FunctorTemp>
        Action::ItrType viacsbHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // Carry/borrow for Integer Add-with-carry Subtract-with-borrow
        template <typename XLEN, OperandMode opMode, bool hasMaskOp, auto detectFuc>
        Action::ItrType vmiacsbHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // Ingeter Compare
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType vmicHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
