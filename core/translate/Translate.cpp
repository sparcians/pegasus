#include "core/translate/Translate.hpp"
#include "core/translate/PageTableEntry.hpp"
#include "core/AtlasInst.hpp"
#include "core/AtlasState.hpp"

#include "include/ActionTags.hpp"

#include "arch/register_macros.hpp"

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
                atlas::Action::createAction<&Translate::translate_<RV64, MMUMode::BAREMETAL>>(
                    this, "Inst Translate (Baremetal)");
            inst_translate_action.addTag(ActionTags::INST_TRANSLATE_TAG);
            inst_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)] =
                inst_translate_action;

            Action data_translate_action =
                atlas::Action::createAction<&Translate::translate_<RV64, MMUMode::BAREMETAL>>(
                    this, "Data Translate (Baremetal)");
            data_translate_action.addTag(ActionTags::DATA_TRANSLATE_TAG);
            data_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)] =
                data_translate_action;
        }
        // Sv32
        {
            Action inst_translate_action =
                atlas::Action::createAction<&Translate::translate_<RV32, MMUMode::SV32>>(
                    this, "Inst Translate (Sv32)");
            inst_translate_action.addTag(ActionTags::INST_TRANSLATE_TAG);
            inst_translation_actions_[static_cast<uint32_t>(MMUMode::SV32)] = inst_translate_action;

            Action data_translate_action =
                atlas::Action::createAction<&Translate::translate_<RV32, MMUMode::SV32>>(
                    this, "Data Translate (Sv32)");
            data_translate_action.addTag(ActionTags::DATA_TRANSLATE_TAG);
            data_translation_actions_[static_cast<uint32_t>(MMUMode::SV32)] = data_translate_action;
        }
        // Sv39
        {
            Action inst_translate_action =
                atlas::Action::createAction<&Translate::translate_<RV64, MMUMode::SV39>>(
                    this, "Inst Translate (Sv39)");
            inst_translate_action.addTag(ActionTags::INST_TRANSLATE_TAG);
            inst_translation_actions_[static_cast<uint32_t>(MMUMode::SV39)] = inst_translate_action;

            Action data_translate_action =
                atlas::Action::createAction<&Translate::translate_<RV64, MMUMode::SV39>>(
                    this, "Data Translate (Sv39)");
            data_translate_action.addTag(ActionTags::DATA_TRANSLATE_TAG);
            data_translation_actions_[static_cast<uint32_t>(MMUMode::SV39)] = data_translate_action;
        }

        // Assume we are booting in Machine mode with translation disabled
        inst_translate_action_group_.addAction(
            inst_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
        data_translate_action_group_.addAction(
            data_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
    }

    void Translate::changeMmuMode(MMUMode mode)
    {
        inst_translate_action_group_.replaceAction(
            ActionTags::INST_TRANSLATE_TAG,
            inst_translation_actions_.at(static_cast<uint32_t>(mode)));
        data_translate_action_group_.replaceAction(
            ActionTags::DATA_TRANSLATE_TAG,
            data_translation_actions_.at(static_cast<uint32_t>(mode)));
    }

    template <typename XLEN, MMUMode Mode> ActionGroup* Translate::translate_(AtlasState* state)
    {
        AtlasTranslationState* translation_state = state->getTranslationState();
        const AtlasTranslationState::TranslationRequest request =
            translation_state->getTranslationRequest();
        const XLEN vaddr = request.virtual_addr;
        ILOG("Translating " << HEX16(vaddr));

        if constexpr (Mode == MMUMode::BAREMETAL)
        {
            translation_state->setTranslationResult(vaddr, request.size);
            // Keep going
            return nullptr;
        }

        // Page size is 4K for both RV32 and RV64
        constexpr uint64_t PAGESIZE = 4096;
        uint64_t ppn = READ_CSR_FIELD(SATP, ppn) * PAGESIZE;
        for (uint32_t level = getNumPageWalkLevels_<Mode>() - 1; level >= 0; --level)
        {
            DLOG("Level " << std::to_string(level) << " Page Walk");

            constexpr uint64_t PTESIZE = sizeof(XLEN);
            const uint64_t vpn = extractVpn_<Mode>(level, vaddr);
            const uint64_t pte_paddr = ppn + (vpn * PTESIZE);
            const PageTableEntry<XLEN, Mode> pte = state->readMemory<XLEN>(pte_paddr);

            DLOG("    Addr: " << HEX16(pte_paddr));
            DLOG("     PTE: " << pte);

            //  If accessing pte violates a PMA or PMP check, raise an access-fault exception
            //  corresponding to the original access type
            if (!pte.isValid() || ((!pte.canRead()) && pte.canWrite()))
            {
                // TODO: Add method to throw correct fault type
                THROW_FETCH_PAGE_FAULT;
            }

            // If PTE is a leaf, perform address translation
            if (pte.isLeaf())
            {
                // TODO: Check access permissions
                const uint64_t paddr =
                    ((uint64_t)pte.getPpn() * PAGESIZE) + extractPageOffset_(request.virtual_addr);
                translation_state->setTranslationResult(paddr, request.size);
                ILOG("  Result: " << HEX16(paddr));

                // Keep going
                return nullptr;
            }
            // Read next level PTE
            else
            {
                ppn = pte.getPpn() * PAGESIZE;
            }
        }

        // If we made it here, it means we didn't find a leaf PTE so translation failed
        // TODO: Add method to throw correct fault type
        THROW_FETCH_PAGE_FAULT;
    }
} // namespace atlas
