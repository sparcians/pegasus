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

        static constexpr std::array<uint32_t, N_MMU_MODES> num_pagewalk_levels_{0, 2, 3, 4, 5};

        template <MMUMode Mode> uint64_t extractVpn_(const uint32_t level, const uint64_t vaddr)
        {
            auto get_vpn_field = [](const uint32_t level) -> const translate_types::FieldDef
            {
                if constexpr (Mode == MMUMode::SV32)
                {
                    return translate_types::Sv32::vpn_fields.at(level);
                }
                else if constexpr (Mode == MMUMode::SV39)
                {
                    return translate_types::Sv39::vpn_fields.at(level);
                }
                else if constexpr (Mode == MMUMode::SV48)
                {
                    return translate_types::Sv48::vpn_fields.at(level);
                }
                else if constexpr (Mode == MMUMode::SV57)
                {
                    return translate_types::Sv57::vpn_fields.at(level);
                }
                else
                {
                    sparta_assert(false, "Unsupported MMU Mode!");
                }
            };

            const translate_types::FieldDef vpn_field = get_vpn_field(level);
            return (vaddr & vpn_field.bitmask) >> vpn_field.lsb;
        }

        uint64_t extractPageOffset_(uint64_t vaddr)
        {
            // Page offset is the same for all MMU modes
            return vaddr & translate_types::Sv32::VAddrFields::page_offset.bitmask;
        }

        friend class ::AtlasTranslateTester;
    };
} // namespace atlas
