#include "core/inst_handlers/b/RvzbkbInsts.hpp"
#include "core/inst_handlers/b/RvbFunctors.hpp"
#include "core/inst_handlers/i/RviFunctors.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/PegasusUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "system/PegasusSystem.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzbkbInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "pack", pegasus::Action::createAction<
                       &RvzbkbInsts::binaryOpHandler_<XLEN, Pack<XLEN>>, RvzbInstsBase>(
                       nullptr, "pack", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "packh", pegasus::Action::createAction<
                       &RvzbkbInsts::binaryOpHandler_<XLEN, Packh<XLEN>>, RvzbInstsBase>(
                       nullptr, "packh", ActionTags::EXECUTE_TAG));

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "packw", pegasus::Action::createAction<
                           &RvzbkbInsts::binaryOpHandler_<XLEN, Packw<XLEN>>, RvzbInstsBase>(
                           nullptr, "packw", ActionTags::EXECUTE_TAG));
        }

        inst_handlers.emplace(
            "brev8", pegasus::Action::createAction<
                       &RvzbkbInsts::unaryOpHandler_<XLEN, Brev8<XLEN>>, RvzbInstsBase>(
                       nullptr, "brev8", ActionTags::EXECUTE_TAG));

        if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "unzip", pegasus::Action::createAction<
                           &RvzbkbInsts::unaryOpHandler_<XLEN, Unzip<XLEN>>, RvzbInstsBase>(
                           nullptr, "unzip", ActionTags::EXECUTE_TAG));

            inst_handlers.emplace(
                "zip", pegasus::Action::createAction<
                           &RvzbkbInsts::unaryOpHandler_<XLEN, Zip<XLEN>>, RvzbInstsBase>(
                           nullptr, "zip", ActionTags::EXECUTE_TAG));
        }

    }

    template void RvzbkbInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbkbInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

} // namespace pegasus
