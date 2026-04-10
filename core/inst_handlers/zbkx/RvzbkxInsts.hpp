#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzbkxInsts
    {
      public:
        using base_type = RvzbkxInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);

      private:
        template <typename XLEN, typename OPERATOR>
        Action::ItrType xpermHandler_(PegasusState* state, Action::ItrType action_it);

        template <typename XLEN> struct Xperm8Op
        {
            XLEN operator()(XLEN rs1, XLEN rs2) const;
        };

        template <typename XLEN> struct Xperm4Op
        {
            XLEN operator()(XLEN rs1, XLEN rs2) const;
        };
    };
} // namespace pegasus
