#pragma once

#include "core/ActionGroup.hpp"

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

      private:
        ActionGroup* translate_(atlas::AtlasState* state);

        ActionGroup inst_translate_action_group_{"Inst Translate"};
        ActionGroup data_translate_action_group_{"Data Translate"};

        friend class ::AtlasTranslateTester;
    };
} // namespace atlas
