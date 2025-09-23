#pragma once

#include "core/inst_handlers/a/RvaInstsBase.hpp"

namespace pegasus
{
    class PegasusState;

    class RvaInsts : public RvaInstsBase
    {
      public:
        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);
    };
} // namespace pegasus
