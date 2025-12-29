#pragma once

#include "core/ActionGroup.hpp"
#include "core/translate/TranslateTypes.hpp"
#include "include/PegasusTypes.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

class PegasusTranslateTester;

namespace pegasus
{
    class PegasusState;
    class PegasusTranslationState;

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

        ActionGroup* getExecuteTranslateActionGroup(const bool hypervisor = false)
        {
            if (SPARTA_EXPECT_FALSE(hypervisor))
            {
                return &hyp_execute_translate_action_group_;
            }
            return &execute_translate_action_group_;
        }

        ActionGroup* getLoadTranslateActionGroup(const bool hypervisor = false)
        {
            if (SPARTA_EXPECT_FALSE(hypervisor))
            {
                return &hyp_load_translate_action_group_;
            }
            return &load_translate_action_group_;
        }

        ActionGroup* getStoreTranslateActionGroup(const bool hypervisor = false)
        {
            if (SPARTA_EXPECT_FALSE(hypervisor))
            {
                return &hyp_store_translate_action_group_;
            }
            return &store_translate_action_group_;
        }

        template <typename XLEN>
        void changeMMUMode(const translate_types::TranslationType type, const MMUMode mode,
                           const MMUMode ls_mode);

      private:
        std::array<MMUMode, translate_types::N_TRANS_TYPES> mmu_modes_;
        std::array<MMUMode, translate_types::N_TRANS_TYPES> mmu_ls_modes_;

        // Translate ActionGroups
        ActionGroup execute_translate_action_group_{"Execute (Inst) Translate"};
        ActionGroup load_translate_action_group_{"Load Translate"};
        ActionGroup store_translate_action_group_{"Store Translate"};

        // Hypervisor Translate ActionGroups
        ActionGroup hyp_execute_translate_action_group_{"Hypervisor Execute Translate"};
        ActionGroup hyp_load_translate_action_group_{"Hypervisor Load Translate"};
        ActionGroup hyp_store_translate_action_group_{"Hypervisor Store Translate"};

        // Translate Actions
        using TranslateActionsType =
            std::array<std::array<Action, N_MMU_MODES>, translate_types::N_ACCESS_TYPES>;
        TranslateActionsType rv64_translation_actions_;
        TranslateActionsType rv32_translation_actions_;

        template <typename XLEN>
        Action & getTranslateAction_(translate_types::AccessType type, MMUMode mode)
        {
            const uint32_t type_idx = static_cast<uint32_t>(type);
            const uint32_t mode_idx = static_cast<uint32_t>(mode);
            return rv64_translation_actions_[type_idx][mode_idx];
        }

        // Hypervisor Translate Actions
        std::array<Action, translate_types::N_ACCESS_TYPES> rv64_hyp_translation_actions_;
        std::array<Action, translate_types::N_ACCESS_TYPES> rv32_hyp_translation_actions_;

        template <typename XLEN, MMUMode MODE, translate_types::AccessType TYPE>
        Action::ItrType translate_(pegasus::PegasusState* state, Action::ItrType action_it);

        template <typename XLEN, MMUMode MODE, translate_types::AccessType TYPE>
        Action::ItrType setResult_(PegasusTranslationState* translation_state,
                                   Action::ItrType action_it, const bool virt_mode,
                                   const Addr paddr, const uint32_t level = 1);

        template <typename XLEN, translate_types::AccessType TYPE>
        Action::ItrType hypervisorTranslate_(pegasus::PegasusState* state,
                                             Action::ItrType action_it);

        template <typename XLEN, MMUMode MODE, translate_types::AccessType TYPE>
        void registerAction_(const char* desc, const ActionTagType tags,
                             TranslateActionsType & xlation_actions)
        {
            Action action =
                Action::createAction<&pegasus::Translate::translate_<XLEN, MODE, TYPE>>(this, desc);
            action.addTag(tags);
            xlation_actions[static_cast<uint32_t>(TYPE)][static_cast<uint32_t>(MODE)] = action;
        }

        friend class ::PegasusTranslateTester;
    };
} // namespace pegasus
