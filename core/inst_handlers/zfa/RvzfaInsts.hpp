#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"
#include "core/inst_handlers/f/RvfInstsBase.hpp"

#include <map>
#include <string>
#include <math.h>

namespace pegasus
{
    class PegasusState;

    class RvzfaInsts : public RvfInstsBase
    {
      public:
        using base_type = RvzfaInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, typename SIZE>
        Action::ItrType fliHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename SIZE, bool ISMAX>
        Action::ItrType fminmaxHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename SIZE, bool EXACT>
        Action::ItrType froundHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        inline static const std::array<uint32_t, 32> fli_table_ = {
            0xBF800000, 0x00800000, 0x37800000, 0x38000000, 0x3B800000, 0x3C000000, 0x3D800000,
            0x3E000000, 0x3E800000, 0x3EA00000, 0x3EC00000, 0x3EE00000, 0x3F000000, 0x3F200000,
            0x3F400000, 0x3F600000, 0X3F800000, 0X3FA00000, 0X3FC00000, 0X3FE00000, 0X40000000,
            0X40200000, 0X40400000, 0X40800000, 0X41000000, 0X41800000, 0X43000000, 0X43800000,
            0X47000000, 0X47800000, 0X7F800000, 0x7FC00000};
    };
} // namespace pegasus
