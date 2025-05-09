#pragma once

#include "core/ActionGroup.hpp"
#include "core/translate/TranslateTypes.hpp"
#include "include/AtlasTypes.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

class AtlasTranslateTester;

namespace atlas
{
    class AtlasState;
    class AtlasTranslationState;

    class Translate : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "Translate";
        using base_type = Translate;

        class TranslateParameters : public sparta::ParameterSet
        {
          public:
            TranslateParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}
        };

        Translate(sparta::TreeNode* translate_node, const TranslateParameters* p);

        ActionGroup* getInstTranslateActionGroup() { return &inst_translate_action_group_; }

        ActionGroup* getLoadTranslateActionGroup() { return &load_translate_action_group_; }

        ActionGroup* getStoreTranslateActionGroup() { return &store_translate_action_group_; }

        template <typename XLEN> void changeMMUMode(const MMUMode mode, const MMUMode ls_mode);

        enum class AccessType
        {
            INSTRUCTION,
            LOAD,
            STORE
        };

      private:
        ActionGroup inst_translate_action_group_{"Inst Translate"};
        ActionGroup load_translate_action_group_{"Load Translate"};
        ActionGroup store_translate_action_group_{"Store Translate"};

        std::array<Action, N_MMU_MODES> rv64_inst_translation_actions_;
        std::array<Action, N_MMU_MODES> rv32_inst_translation_actions_;
        std::array<Action, N_MMU_MODES> rv64_load_translation_actions_;
        std::array<Action, N_MMU_MODES> rv32_load_translation_actions_;
        std::array<Action, N_MMU_MODES> rv64_store_translation_actions_;
        std::array<Action, N_MMU_MODES> rv32_store_translation_actions_;

        template <typename XLEN, MMUMode MODE, AccessType TYPE>
        Action* translate_(atlas::AtlasState* state, Action* action);

        template <typename XLEN, MMUMode MODE, AccessType TYPE>
        Action* setResult_(AtlasTranslationState* translation_state, Action* action,
                           const Addr paddr, const uint32_t level = 1);

        template <typename XLEN, MMUMode MODE, AccessType TYPE>
        void registerAction_(const char* desc, const ActionTagType tags,
                             std::array<Action, N_MMU_MODES> & xlation_actions)
        {
            Action action =
                Action::createAction<&atlas::Translate::translate_<XLEN, MODE, TYPE>>(this, desc);
            action.addTag(tags);
            xlation_actions[static_cast<uint32_t>(MODE)] = action;
        }

        friend class ::AtlasTranslateTester;
    };
} // namespace atlas
