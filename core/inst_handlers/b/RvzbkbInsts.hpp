#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"
#include "core/inst_handlers/b/RvzbInstsBase.hpp"

#include <map>
#include <string>
#include <bit>

namespace pegasus
{
    class PegasusState;

    class RvzbkbInsts : public RvzbInstsBase
    {
      public:
        using base_type = RvzbkbInsts;

        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);
    };
} // namespace pegasus
