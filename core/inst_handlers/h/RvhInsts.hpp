#pragma once

#include "core/InstHandlers.hpp"
#include "include/PegasusTranslateTypes.hpp"

namespace pegasus
{
    class PegasusState;

    class RvhInsts
    {
      public:
        using base_type = RvhInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(InstHandlers::InstHandlersMap &);

        template <typename XLEN> static void getInstHandlers(InstHandlers::InstHandlersMap &);

      private:
        // hfence.vvma, hfence.gvma
        template <typename XLEN, bool GUEST>
        Action::ItrType hfenceHandler_(PegasusState*, Action::ItrType);

        // compute address handler
        template <typename XLEN, typename SIZE, translate_types::AccessType ACCESS_TYPE>
        Action::ItrType computeAddressHandler_(PegasusState*, Action::ItrType);

        // hlv/hlvcx
        template <typename XLEN, typename SIZE, bool SIGN_EXTEND = false>
        Action::ItrType loadHandler_(PegasusState*, Action::ItrType);

        // hsv
        template <typename XLEN, typename SIZE>
        Action::ItrType storeHandler_(PegasusState*, Action::ItrType);
    };
} // namespace pegasus
