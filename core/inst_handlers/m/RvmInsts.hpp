#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvmInsts
    {
      public:
        using base_type = RvmInsts;

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);

      private:
        // div
        template <typename XLEN>
        Action::ItrType divHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // divu
        template <typename XLEN>
        Action::ItrType divuHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // divw (rv64 only)
        Action::ItrType divuwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // divw (rv64 only)
        Action::ItrType divwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // mul, mulh, mulhsu, mulhu
        template <typename XLEN, typename OPERATOR>
        Action::ItrType mulHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // mulw (rv64 only)
        Action::ItrType mulwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // rem
        template <typename XLEN>
        Action::ItrType remHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // remu
        template <typename XLEN>
        Action::ItrType remuHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // remuw (rv64 only)
        Action::ItrType remuwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // remw (rv64 only)
        Action::ItrType remwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
