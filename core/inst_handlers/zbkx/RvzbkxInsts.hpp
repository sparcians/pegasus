#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus {
    class PegasusState;

    class RvzbkxInsts{
    public:
        using base_type = RvzbkxInsts;

        template<typename XLEN>
        static void getInstHandlers(InstHandlers::InstHandlersMap &);

    private:

        template<typename XLEN, typename OPERATOR>
        Action::ItrType xpermHandler_(PegasusState* state, Action::ItrType action_it);

        struct Xperm8Op {
            template<typename XLEN>
            XLEN operator()(XLEN rs1, XLEN rs2) const;
        };

        struct Xperm4Op{
            template <typename XLEN>
            XLEN operator()(XLEN rs1, XLEN rs2) const;
        };
    };
}
