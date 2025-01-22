#include "core/translate/Translate.hpp"
#include "core/AtlasInst.hpp"
#include "core/AtlasState.hpp"
#include "include/ActionTags.hpp"

#include "sparta/utils/LogUtils.hpp"

namespace atlas
{
    Translate::Translate(sparta::TreeNode* translate_node, const TranslateParameters* p) :
        sparta::Unit(translate_node)
    {
        (void)p;

        // Baremetal (translation disabled)
        {
            Action inst_translate_action =
                atlas::Action::createAction<&Translate::translate_<MMUMode::BAREMETAL>>(
                    this, "Inst Translate (Baremetal)");
            inst_translate_action.addTag(ActionTags::INST_TRANSLATE_TAG);
            inst_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)] =
                inst_translate_action;

            Action data_translate_action =
                atlas::Action::createAction<&Translate::translate_<MMUMode::BAREMETAL>>(
                    this, "Data Translate (Baremetal)");
            data_translate_action.addTag(ActionTags::DATA_TRANSLATE_TAG);
            data_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)] =
                data_translate_action;
        }

        // Assume we are booting in Machine mode with translation disabled
        inst_translate_action_group_.addAction(
            inst_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
        data_translate_action_group_.addAction(
            data_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
    }

    template <MMUMode Mode> ActionGroup* Translate::translate_(AtlasState* state)
    {
        AtlasTranslationState* translation_state = state->getTranslationState();
        const AtlasTranslationState::TranslationRequest request =
            translation_state->getTranslationRequest();

        ILOG("Translating 0x" << std::hex << request.virtual_addr);

        if constexpr (Mode == MMUMode::BAREMETAL)
        {
            translation_state->setTranslationResult(request.virtual_addr, request.size);
        }
        else if constexpr (Mode == MMUMode::SV32)
        {
            sparta_assert(false, "Sv32 translation is not supported!");
        }
        else if constexpr (Mode == MMUMode::SV39)
        {
            sparta_assert(false, "Sv39 translation is not supported!");
        }
        else if constexpr (Mode == MMUMode::SV48)
        {
            sparta_assert(false, "Sv48 translation is not supported!");
        }
        else if constexpr (Mode == MMUMode::SV57)
        {
            sparta_assert(false, "Sv57 translation is not supported!");
        }

        // Keep going
        return nullptr;
    }
} // namespace atlas
