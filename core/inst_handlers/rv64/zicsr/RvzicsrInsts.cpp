#include "core/inst_handlers/rv64/zicsr/RvzicsrInsts.hpp"
#include "core/ActionGroup.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    void RvzicsrInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace(
            "csrrc", atlas::Action::createAction<&RvzicsrInsts::csrrc_64_handler, RvzicsrInsts>(
                         nullptr, "csrrc", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrci", atlas::Action::createAction<&RvzicsrInsts::csrrci_64_handler, RvzicsrInsts>(
                          nullptr, "csrrci", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrs", atlas::Action::createAction<&RvzicsrInsts::csrrs_64_handler, RvzicsrInsts>(
                         nullptr, "csrrs", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrsi", atlas::Action::createAction<&RvzicsrInsts::csrrsi_64_handler, RvzicsrInsts>(
                          nullptr, "csrrsi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrw", atlas::Action::createAction<&RvzicsrInsts::csrrw_64_handler, RvzicsrInsts>(
                         nullptr, "csrrw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrwi", atlas::Action::createAction<&RvzicsrInsts::csrrwi_64_handler, RvzicsrInsts>(
                          nullptr, "csrrwi", ActionTags::EXECUTE_TAG));
    }
}; // namespace atlas
