#pragma once

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvviaInsts
    {
      public:
        using base_type = RvviaInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, typename VLEN, template<typename> typename OP>
        ActionGroup* viavv_handler_(atlas::AtlasState* state);
        template <typename XLEN, typename VLEN, template<typename> typename OP>
        ActionGroup* viavx_handler_(atlas::AtlasState* state);
        template <typename XLEN, typename VLEN, template<typename> typename OP>
        ActionGroup* viavi_handler_(atlas::AtlasState* state);
    };
} // namespace atlas
