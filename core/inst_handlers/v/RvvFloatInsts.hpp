#pragma once

#include <map>
#include <string>
#include <stdint.h>
#include <tuple>

#include "core/Action.hpp"
#include "core/inst_handlers/vector_types.hpp"

namespace pegasus
{
    class PegasusState;
    class Action;
    class ActionGroup;

    class RvvFloatInsts
    {
      public:
        using base_type = RvvFloatInsts;

        template <auto F16, auto F32, auto F64> struct FloatFuncs
        {
            static constexpr auto f16 = F16;
            static constexpr auto f32 = F32;
            static constexpr auto f64 = F64;
        };

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN>
        Action::ItrType vfmvHandler_(pegasus::PegasusState* state_ptr, Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType vfmergeHandler_(pegasus::PegasusState* state_ptr,
                                        Action::ItrType action_it);

        template <typename XLEN6, OperandMode opMode, typename funcs,
                  RoundingMode rm = RoundingMode::DYN>
        Action::ItrType vfUnaryHandler_(pegasus::PegasusState* state_ptr,
                                        Action::ItrType action_it);

        // FloatToInt is implemented seperately from Unary becuase functions in softfloat have 3
        // arguments instead of 1.
        template <typename XLEN, OperandMode opMode, RvvFloatInsts::FloatFuncs funcs,
                  RoundingMode rm = RoundingMode::DYN>
        Action::ItrType vfFloatToIntHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, FloatFuncs funcs>
        Action::ItrType vfBinaryHandler_(pegasus::PegasusState* state_ptr,
                                         Action::ItrType action_it);

        template <typename XLEN, FloatFuncs funcs>
        Action::ItrType vfrBinaryHandler_(pegasus::PegasusState* state_ptr,
                                          Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, FloatFuncs funcs>
        Action::ItrType vmfBinaryHandler_(pegasus::PegasusState* state_ptr,
                                          Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, template <typename> typename FuncT>
        Action::ItrType vfTernaryHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
