#pragma once

#include <map>
#include <string>
#include <stdint.h>
#include <tuple>

#include "core/Action.hpp"
#include "core/inst_handlers/vector_types.hpp"

extern "C"
{
#include "source/RISCV/specialize.h"
#include "source/include/internals.h"
}

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvvFloatInsts
    {
      public:
        using base_type = RvvFloatInsts;

        template <typename F16, typename F32, typename F64> struct FloatFuncs
        {
            const F16 f16;
            const F32 f32;
            const F64 f64;

            constexpr FloatFuncs(F16 f16, F32 f32, F64 f64) : f16(f16), f32(f32), f64(f64) {}
        };

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN>
        Action::ItrType vfmvHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);

        template <typename XLEN>
        Action::ItrType vfmergeHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, FloatFuncs funcs>
        Action::ItrType vfUnaryHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, RvvFloatInsts::FloatFuncs funcs>
        Action::ItrType vfFloatToIntHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, FloatFuncs funcs>
        Action::ItrType vfBinaryHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);

        template <typename XLEN, FloatFuncs funcs>
        Action::ItrType vfrBinaryHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, FloatFuncs funcs>
        Action::ItrType vmfBinaryHandler_(atlas::AtlasState* state_ptr, Action::ItrType action_it);

        template <typename XLEN, OperandMode opMode, auto funcWrapper>
        Action::ItrType vfTernaryHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
