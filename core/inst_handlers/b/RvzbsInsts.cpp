#include "core/inst_handlers/b/RvzbsInsts.hpp"
#include "core/inst_handlers/b/RvbFunctors.hpp"
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
    void RvzbsInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "bclr",
            pegasus::Action::createAction<&RvzbsInsts::binaryOpHandler_<XLEN, Bclr<XLEN>>,
                                          RvzbInstsBase>(nullptr, "bclr", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bclri", pegasus::Action::createAction<&RvzbsInsts::immOpHandler_<XLEN, Bclr<XLEN>>,
                                                   RvzbInstsBase>(nullptr, "bclri",
                                                                  ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bext",
            pegasus::Action::createAction<&RvzbsInsts::binaryOpHandler_<XLEN, Bext<XLEN>>,
                                          RvzbInstsBase>(nullptr, "bext", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bexti", pegasus::Action::createAction<&RvzbsInsts::immOpHandler_<XLEN, Bext<XLEN>>,
                                                   RvzbInstsBase>(nullptr, "bexti",
                                                                  ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "binv",
            pegasus::Action::createAction<&RvzbsInsts::binaryOpHandler_<XLEN, Binv<XLEN>>,
                                          RvzbInstsBase>(nullptr, "binv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "binvi", pegasus::Action::createAction<&RvzbsInsts::immOpHandler_<XLEN, Binv<XLEN>>,
                                                   RvzbInstsBase>(nullptr, "binvi",
                                                                  ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bset",
            pegasus::Action::createAction<&RvzbsInsts::binaryOpHandler_<XLEN, Bset<XLEN>>,
                                          RvzbInstsBase>(nullptr, "bset", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bseti", pegasus::Action::createAction<&RvzbsInsts::immOpHandler_<XLEN, Bset<XLEN>>,
                                                   RvzbInstsBase>(nullptr, "bseti",
                                                                  ActionTags::EXECUTE_TAG));
    }

    template void RvzbsInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbsInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

} // namespace pegasus
