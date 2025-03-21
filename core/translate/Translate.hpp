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

        void changeMmuMode(const uint64_t xlen, MMUMode mode);

        MMUMode getMode() const { return mode_; }

      private:
        MMUMode mode_ = MMUMode::BAREMETAL;
        ActionGroup inst_translate_action_group_{"Inst Translate"};
        ActionGroup data_translate_action_group_{"Data Translate"};

        std::array<Action, N_MMU_MODES> rv64_inst_translation_actions_;
        std::array<Action, N_MMU_MODES> rv32_inst_translation_actions_;
        std::array<Action, N_MMU_MODES> rv64_data_translation_actions_;
        std::array<Action, N_MMU_MODES> rv32_data_translation_actions_;

        template <typename XLEN, MMUMode Mode, bool>
        ActionGroup* translate_(atlas::AtlasState* state);

        template <MMUMode Mode> uint32_t getNumPageWalkLevels_() const
        {
            if constexpr (Mode == MMUMode::SV32)
            {
                return translate_types::Sv32::num_pagewalk_levels;
            }
            else if constexpr (Mode == MMUMode::SV39)
            {
                return translate_types::Sv39::num_pagewalk_levels;
            }
            else if constexpr (Mode == MMUMode::SV48)
            {
                return translate_types::Sv48::num_pagewalk_levels;
            }
            else if constexpr (Mode == MMUMode::SV57)
            {
                return translate_types::Sv57::num_pagewalk_levels;
            }
            else
            {
                sparta_assert(false, "Unsupported MMU Mode!");
            }
        }

        template <MMUMode Mode> uint64_t extractVpn_(const uint32_t level, const uint64_t vaddr)
        {
            auto get_vpn_field = [](const uint32_t level) -> const translate_types::FieldDef &
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

            const translate_types::FieldDef & vpn_field = get_vpn_field(level);
            return (vaddr & vpn_field.bitmask) >> vpn_field.lsb;
        }

        uint64_t extractPageOffset_(uint64_t vaddr)
        {
            // Page offset is the same for all MMU modes
            return vaddr & translate_types::Sv32::VAddrFields::page_offset.bitmask;
        }

        template <typename XLEN, MMUMode MODE, bool INST_TRANSLATION>
        void registerAction_(const char* desc, const ActionTagType tags,
                             std::array<Action, N_MMU_MODES> & xlation_actions)
        {
            Action action =
                Action::createAction<&atlas::Translate::translate_<XLEN, MODE, INST_TRANSLATION>>(
                    this, desc);
            action.addTag(tags);
            xlation_actions[static_cast<uint32_t>(MODE)] = action;
        }

        friend class ::AtlasTranslateTester;
    };
} // namespace atlas
