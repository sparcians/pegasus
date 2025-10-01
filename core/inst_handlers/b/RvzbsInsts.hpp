#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"
#include "core/inst_handlers/b/RvzbInstsBase.hpp"

#include <map>
#include <string>

namespace pegasus
{
    class PegasusState;

    class RvzbsInsts : public RvzbInstsBase
    {
      public:
        using base_type = RvzbsInsts;

        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);
    };
} // namespace pegasus
