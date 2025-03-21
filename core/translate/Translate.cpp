#include "core/translate/Translate.hpp"
#include "core/translate/PageTableEntry.hpp"
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

        constexpr bool INST_TRANSLATION = true;
        constexpr bool DATA_TRANSLATION = false;

        // Baremetal (translation disabled)
        {
            registerAction_<RV32, MMUMode::BAREMETAL, INST_TRANSLATION>(
                "Inst Translate (Baremetal)", ActionTags::INST_TRANSLATE_TAG,
                rv32_inst_translation_actions_);

            registerAction_<RV32, MMUMode::BAREMETAL, DATA_TRANSLATION>(
                "Data Translate (Baremetal)", ActionTags::DATA_TRANSLATE_TAG,
                rv32_data_translation_actions_);

            // RV64
            registerAction_<RV64, MMUMode::BAREMETAL, INST_TRANSLATION>(
                "Inst Translate (Baremetal)", ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_);

            registerAction_<RV64, MMUMode::BAREMETAL, DATA_TRANSLATION>(
                "Data Translate (Baremetal)", ActionTags::DATA_TRANSLATE_TAG,
                rv64_data_translation_actions_);
        }

        // Sv32
        {
            registerAction_<RV32, MMUMode::SV32, INST_TRANSLATION>("Inst Translate (Sv32)",
                                                                   ActionTags::INST_TRANSLATE_TAG,
                                                                   rv32_inst_translation_actions_);
            registerAction_<RV32, MMUMode::SV32, DATA_TRANSLATION>("Data Translate (Sv32)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv32_data_translation_actions_);
            registerAction_<RV64, MMUMode::SV32, INST_TRANSLATION>("Inst Translate (Sv32)",
                                                                   ActionTags::INST_TRANSLATE_TAG,
                                                                   rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV32, DATA_TRANSLATION>("Data Translate (Sv32)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_data_translation_actions_);
        }
        // Sv39
        {
            registerAction_<RV64, MMUMode::SV39, INST_TRANSLATION>("Inst Translate (Sv39)",
                                                                   ActionTags::INST_TRANSLATE_TAG,
                                                                   rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV39, DATA_TRANSLATION>("Data Translate (Sv39)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_data_translation_actions_);
        }

        // Assume we are booting in RV64 Machine mode with translation disabled
        inst_translate_action_group_.addAction(
            rv64_inst_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
        data_translate_action_group_.addAction(
            rv64_data_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
    }

    void Translate::changeMMUMode(const uint64_t xlen, const MMUMode mode)
    {
        if (xlen == 64)
        {
            inst_translate_action_group_.replaceAction(
                ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_.at(static_cast<uint32_t>(mode)));
            data_translate_action_group_.replaceAction(
                ActionTags::DATA_TRANSLATE_TAG,
                rv64_data_translation_actions_.at(static_cast<uint32_t>(mode)));
        }
        else
        {
            inst_translate_action_group_.replaceAction(
                ActionTags::INST_TRANSLATE_TAG,
                rv32_inst_translation_actions_.at(static_cast<uint32_t>(mode)));
            data_translate_action_group_.replaceAction(
                ActionTags::DATA_TRANSLATE_TAG,
                rv32_data_translation_actions_.at(static_cast<uint32_t>(mode)));
        }
    }

    template <typename XLEN, MMUMode MODE, bool INST_TRANSLATION>
    ActionGroup* Translate::translate_(AtlasState* state)
    {
        AtlasTranslationState* translation_state = nullptr;
        if constexpr (INST_TRANSLATION)
        {
            // Translation reqest is from fetch
            translation_state = state->getFetchTranslationState();
        }
        else
        {
            const auto & inst = state->getCurrentInst();
            translation_state = inst->getTranslationState();
        }

        const AtlasTranslationState::TranslationRequest request = translation_state->getRequest();
        const XLEN vaddr = request.getVaddr();

        if constexpr (MODE == MMUMode::BAREMETAL)
        {
            translation_state->setResult(vaddr, request.getSize());
            // Keep going
            return nullptr;
        }

        const uint32_t width = std::is_same_v<XLEN, RV64> ? 16 : 8;
        ILOG("Translating " << HEX(vaddr, width));

        // Page size is 4K for both RV32 and RV64
        constexpr uint64_t PAGESIZE = 4096;
        uint64_t ppn = READ_CSR_FIELD<XLEN>(state, SATP, "ppn") * PAGESIZE;
        for (uint32_t level = getNumPageWalkLevels_<MODE>(); level > 0; --level)
        {
            constexpr uint64_t PTESIZE = sizeof(XLEN);
            const auto indexed_level = level - 1;
            const uint64_t vpn = extractVpn_<MODE>(indexed_level, vaddr);
            const uint64_t pte_paddr = ppn + (vpn * PTESIZE);
            const PageTableEntry<XLEN, MODE> pte = state->readMemory<XLEN>(pte_paddr);
            DLOG_CODE_BLOCK(DLOG_OUTPUT("Level " << std::to_string(indexed_level) << " Page Walk");
                            DLOG_OUTPUT("    Addr: " << HEX(pte_paddr, width));
                            DLOG_OUTPUT("     PTE: " << pte););

            //  If accessing pte violates a PMA or PMP check, raise an
            //  access-fault exception corresponding to the original
            //  access type
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
                    ((uint64_t)pte.getPpn() * PAGESIZE) + extractPageOffset_(request.getVaddr());
                translation_state->setResult(paddr, request.getSize());
                ILOG("  Result: " << HEX(paddr, width));

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

    // Being pedantic
    template ActionGroup* Translate::translate_<RV32, MMUMode::BAREMETAL, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::BAREMETAL, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV32, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV32, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV39, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV39, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV48, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV48, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV57, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV32, MMUMode::SV57, false>(AtlasState*);

    template ActionGroup* Translate::translate_<RV64, MMUMode::BAREMETAL, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::BAREMETAL, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV32, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV32, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV39, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV39, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV48, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV48, false>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV57, true>(AtlasState*);
    template ActionGroup* Translate::translate_<RV64, MMUMode::SV57, false>(AtlasState*);

} // namespace atlas
