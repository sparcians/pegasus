#pragma once

#include "core/ActionGroup.hpp"

#include "include/AtlasTypes.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

class AtlasTranslateTester;

namespace atlas
{
    class AtlasState;

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

        ActionGroup* getDataTranslateActionGroup() { return &data_translate_action_group_; }

        void changeMmuMode(MMUMode mode);

      private:
        template <typename XLEN, MMUMode Mode> ActionGroup* translate_(atlas::AtlasState* state);

        ActionGroup inst_translate_action_group_{"Inst Translate"};
        ActionGroup data_translate_action_group_{"Data Translate"};

        std::array<Action, N_MMU_MODES> inst_translation_actions_;
        std::array<Action, N_MMU_MODES> data_translation_actions_;

        template <MMUMode Mode> uint32_t getNumPageWalkLevels_() const
        {
            return num_pagewalk_levels_.at(static_cast<uint32_t>(Mode));
        }

        const std::array<uint32_t, N_MMU_MODES> num_pagewalk_levels_{0, 2, 3, 4, 5};

        friend class ::AtlasTranslateTester;
    };
} // namespace atlas
