#include "core/inst_handlers/rv64/m/RvmInsts.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    void RvmInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace("div",
                              atlas::Action::createAction<&RvmInsts::div_64_handler, RvmInsts>(
                                  nullptr, "div", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("divu",
                              atlas::Action::createAction<&RvmInsts::divu_64_handler, RvmInsts>(
                                  nullptr, "divu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("divuw",
                              atlas::Action::createAction<&RvmInsts::divuw_64_handler, RvmInsts>(
                                  nullptr, "divuw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("divw",
                              atlas::Action::createAction<&RvmInsts::divw_64_handler, RvmInsts>(
                                  nullptr, "divw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mul",
                              atlas::Action::createAction<&RvmInsts::mul_64_handler, RvmInsts>(
                                  nullptr, "mul", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mulh",
                              atlas::Action::createAction<&RvmInsts::mulh_64_handler, RvmInsts>(
                                  nullptr, "mulh", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mulhsu",
                              atlas::Action::createAction<&RvmInsts::mulhsu_64_handler, RvmInsts>(
                                  nullptr, "mulhsu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mulhu",
                              atlas::Action::createAction<&RvmInsts::mulhu_64_handler, RvmInsts>(
                                  nullptr, "mulhu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("mulw",
                              atlas::Action::createAction<&RvmInsts::mulw_64_handler, RvmInsts>(
                                  nullptr, "mulw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("rem",
                              atlas::Action::createAction<&RvmInsts::rem_64_handler, RvmInsts>(
                                  nullptr, "rem", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("remu",
                              atlas::Action::createAction<&RvmInsts::remu_64_handler, RvmInsts>(
                                  nullptr, "remu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("remuw",
                              atlas::Action::createAction<&RvmInsts::remuw_64_handler, RvmInsts>(
                                  nullptr, "remuw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("remw",
                              atlas::Action::createAction<&RvmInsts::remw_64_handler, RvmInsts>(
                                  nullptr, "remw", ActionTags::EXECUTE_TAG));
    }
}; // namespace atlas
