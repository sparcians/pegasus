#include "include/ActionTags.hpp"
#include "core/ActionTagFactory.hpp"

namespace pegasus
{
    // Core Actions
    const ActionTagType ActionTags::FETCH_TAG = ActionTagFactory::createTag("FETCH");
    const ActionTagType ActionTags::INST_S_STAGE_TRANSLATE_TAG =
        ActionTagFactory::createTag("INST_S_STAGE_TRANSLATE");
    const ActionTagType ActionTags::INST_VS_STAGE_TRANSLATE_TAG =
        ActionTagFactory::createTag("INST_VS_STAGE_TRANSLATE");
    const ActionTagType ActionTags::INST_G_STAGE_TRANSLATE_TAG =
        ActionTagFactory::createTag("INST_G_STAGE_TRANSLATE");
    const ActionTagType ActionTags::DECODE_TAG = ActionTagFactory::createTag("DECODE");
    const ActionTagType ActionTags::EXECUTE_TAG = ActionTagFactory::createTag("EXECUTE");
    const ActionTagType ActionTags::COMPUTE_ADDR_TAG = ActionTagFactory::createTag("COMPUTE_ADDR");
    const ActionTagType ActionTags::DATA_S_STAGE_TRANSLATE_TAG =
        ActionTagFactory::createTag("DATA_S_STAGE_TRANSLATE");
    const ActionTagType ActionTags::DATA_VS_STAGE_TRANSLATE_TAG =
        ActionTagFactory::createTag("DATA_VS_STAGE_TRANSLATE");
    const ActionTagType ActionTags::DATA_G_STAGE_TRANSLATE_TAG =
        ActionTagFactory::createTag("DATA_G_STAGE_TRANSLATE");
    const ActionTagType ActionTags::EXCEPTION_TAG = ActionTagFactory::createTag("EXCEPTION");

    // Stop Simulation
    const ActionTagType ActionTags::STOP_SIM_TAG = ActionTagFactory::createTag("STOP_SIM");
} // namespace pegasus
