
#include <vector>

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

        // Baremetal (translation disabled)
        {
            registerAction_<RV32, MMUMode::BAREMETAL, AccessType::INSTRUCTION>(
                "Inst Translate (Baremetal)", ActionTags::INST_TRANSLATE_TAG,
                rv32_inst_translation_actions_);
            registerAction_<RV32, MMUMode::BAREMETAL, AccessType::LOAD>(
                "Load Translate (Baremetal)", ActionTags::DATA_TRANSLATE_TAG,
                rv32_load_translation_actions_);
            registerAction_<RV32, MMUMode::BAREMETAL, AccessType::STORE>(
                "Store Translate (Baremetal)", ActionTags::DATA_TRANSLATE_TAG,
                rv32_store_translation_actions_);
            registerAction_<RV64, MMUMode::BAREMETAL, AccessType::INSTRUCTION>(
                "Inst Translate (Baremetal)", ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::BAREMETAL, AccessType::LOAD>(
                "Load Translate (Baremetal)", ActionTags::DATA_TRANSLATE_TAG,
                rv64_load_translation_actions_);
            registerAction_<RV64, MMUMode::BAREMETAL, AccessType::STORE>(
                "Store Translate (Baremetal)", ActionTags::DATA_TRANSLATE_TAG,
                rv64_store_translation_actions_);
        }

        // Sv32
        {
            registerAction_<RV32, MMUMode::SV32, AccessType::INSTRUCTION>(
                "Inst Translate (Sv32)", ActionTags::INST_TRANSLATE_TAG,
                rv32_inst_translation_actions_);
            registerAction_<RV32, MMUMode::SV32, AccessType::LOAD>("Load Translate (Sv32)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv32_load_translation_actions_);
            registerAction_<RV32, MMUMode::SV32, AccessType::STORE>(
                "Store Translate (Sv32)", ActionTags::DATA_TRANSLATE_TAG,
                rv32_store_translation_actions_);
            registerAction_<RV64, MMUMode::SV32, AccessType::INSTRUCTION>(
                "Inst Translate (Sv32)", ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV32, AccessType::LOAD>("Load Translate (Sv32)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_load_translation_actions_);
            registerAction_<RV64, MMUMode::SV32, AccessType::STORE>(
                "Store Translate (Sv32)", ActionTags::DATA_TRANSLATE_TAG,
                rv64_store_translation_actions_);
        }
        // Sv39
        {
            registerAction_<RV64, MMUMode::SV39, AccessType::INSTRUCTION>(
                "Inst Translate (Sv39)", ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV39, AccessType::LOAD>("Load Translate (Sv39)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_load_translation_actions_);
            registerAction_<RV64, MMUMode::SV39, AccessType::STORE>(
                "Store Translate (Sv39)", ActionTags::DATA_TRANSLATE_TAG,
                rv64_store_translation_actions_);
        }
        // Sv48
        {
            registerAction_<RV64, MMUMode::SV48, AccessType::INSTRUCTION>(
                "Inst Translate (Sv48)", ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV48, AccessType::LOAD>("Load Translate (Sv48)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_load_translation_actions_);
            registerAction_<RV64, MMUMode::SV48, AccessType::STORE>(
                "Store Translate (Sv48)", ActionTags::DATA_TRANSLATE_TAG,
                rv64_store_translation_actions_);
        }
        // Sv57
        {
            registerAction_<RV64, MMUMode::SV57, AccessType::INSTRUCTION>(
                "Inst Translate (Sv57)", ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_);
            registerAction_<RV64, MMUMode::SV57, AccessType::LOAD>("Load Translate (Sv57)",
                                                                   ActionTags::DATA_TRANSLATE_TAG,
                                                                   rv64_load_translation_actions_);
            registerAction_<RV64, MMUMode::SV57, AccessType::STORE>(
                "Store Translate (Sv57)", ActionTags::DATA_TRANSLATE_TAG,
                rv64_store_translation_actions_);
        }

        // Assume we are booting in RV64 Machine mode with translation disabled
        inst_translate_action_group_.addAction(
            rv64_inst_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
        load_translate_action_group_.addAction(
            rv64_load_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
        store_translate_action_group_.addAction(
            rv64_store_translation_actions_[static_cast<uint32_t>(MMUMode::BAREMETAL)]);
    }

    template <typename XLEN>
    void Translate::changeMMUMode(const MMUMode mode, const MMUMode ls_mode)
    {
        sparta_assert(mode != MMUMode::INVALID);
        sparta_assert(ls_mode != MMUMode::INVALID);

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_translate_action_group_.replaceAction(
                ActionTags::INST_TRANSLATE_TAG,
                rv64_inst_translation_actions_.at(static_cast<uint32_t>(mode)));
            load_translate_action_group_.replaceAction(
                ActionTags::DATA_TRANSLATE_TAG,
                rv64_load_translation_actions_.at(static_cast<uint32_t>(ls_mode)));
            store_translate_action_group_.replaceAction(
                ActionTags::DATA_TRANSLATE_TAG,
                rv64_store_translation_actions_.at(static_cast<uint32_t>(ls_mode)));
        }
        else
        {
            inst_translate_action_group_.replaceAction(
                ActionTags::INST_TRANSLATE_TAG,
                rv32_inst_translation_actions_.at(static_cast<uint32_t>(mode)));
            load_translate_action_group_.replaceAction(
                ActionTags::DATA_TRANSLATE_TAG,
                rv32_load_translation_actions_.at(static_cast<uint32_t>(ls_mode)));
            store_translate_action_group_.replaceAction(
                ActionTags::DATA_TRANSLATE_TAG,
                rv32_store_translation_actions_.at(static_cast<uint32_t>(ls_mode)));
        }
    }

    template void Translate::changeMMUMode<RV32>(const MMUMode, const MMUMode);
    template void Translate::changeMMUMode<RV64>(const MMUMode, const MMUMode);

    template <typename XLEN, MMUMode MODE, Translate::AccessType TYPE>
    ActionGroup* Translate::translate_(AtlasState* state)
    {
        AtlasTranslationState* translation_state = nullptr;
        if constexpr (TYPE == AccessType::INSTRUCTION)
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

        uint32_t level = getNumPageWalkLevels_<MODE>();

        const auto priv_mode =
            (TYPE == AccessType::INSTRUCTION) ? state->getPrivMode() : state->getLdstPrivMode();

        // See if xlation is disable -- no level walks
        if (level == 0 || (priv_mode == PrivMode::MACHINE))
        {
            translation_state->setResult(vaddr, request.getSize());
            // Keep going
            return nullptr;
        }

        const uint32_t width = std::is_same_v<XLEN, RV64> ? 16 : 8;
        ILOG("Translating " << HEX(vaddr, width));

        // Page size is 4K for both RV32 and RV64
        constexpr uint64_t PAGESHIFT = 12; // 4096
        uint64_t ppn = READ_CSR_FIELD<XLEN>(state, SATP, "ppn") << PAGESHIFT;
        while (level > 0)
        {
            const auto indexed_level = level - 1;
            const auto & vpn_field = extractVpnField_<MODE>(indexed_level);
            const uint64_t pte_paddr = ppn + vpn_field.calcPTEOffset(vaddr) * sizeof(XLEN);
            PageTableEntry<XLEN, MODE> pte = state->readMemory<XLEN>(pte_paddr);
            DLOG_CODE_BLOCK(DLOG_OUTPUT("Level " << indexed_level << " Page Walk");
                            DLOG_OUTPUT("    Addr: " << HEX(pte_paddr, width));
                            DLOG_OUTPUT("     PTE: " << pte););

            // If accessing pte violates a PMA or PMP check, raise an
            // access-fault exception corresponding to the original
            // access type
            if ((pte.isValid() == false) || ((pte.canRead() == false) && pte.canWrite()))
            {
                DLOG("Translation FAILED! PTE is not valid");
                break;
            }

            // If PTE is a leaf, perform address translation
            if (pte.isLeaf())
            {
                const uint32_t sum_val = READ_CSR_FIELD<XLEN>(state, MSTATUS, "sum");
                if ((sum_val == 0) && (false == pte.isUserMode())
                    && (priv_mode != PrivMode::SUPERVISOR))
                {
                    // Must throw
                    DLOG("Translation FAILED! Cannot access User mode PTE");
                    break;
                }

                // TODO: Check access permissions more better...
                if constexpr (TYPE == AccessType::STORE)
                {
                    if (false == pte.canWrite())
                    {
                        DLOG("Translation FAILED! PTE does not have write access");
                        THROW_STORE_AMO_PAGE_FAULT;
                    }
                }
                else if constexpr (TYPE == AccessType::LOAD)
                {
                    if (false == pte.canRead())
                    {
                        DLOG("Translation FAILED! PTE does not have read access");
                        THROW_LOAD_PAGE_FAULT;
                    }
                }

                constexpr bool is_store = TYPE == AccessType::STORE;
                if (false == pte.isAccessable(is_store))
                {
                    // See if we're required to update access bits in the PTE
                    if (READ_CSR_FIELD<XLEN>(state, MENVCFG, "adue"))
                    {
                        if constexpr (TYPE == AccessType::STORE)
                        {
                            pte.setDirty();
                        }
                        pte.setAccessed();
                        state->writeMemory<XLEN>(pte_paddr, pte.getPte());
                    }
                    else
                    {
                        // Take exception -- no access allowed or not dirty
                        DLOG("Translation FAILED: Cannot access dirty page");
                        break;
                    }
                }
                const auto index_bits = (vpn_field.msb - vpn_field.lsb + 1) * indexed_level;
                const auto virt_base = vaddr >> PAGESHIFT;
                Addr paddr = (Addr(pte.getPpn()) | (virt_base & ((0b1 << index_bits) - 1)))
                             << PAGESHIFT;
                paddr |= extractPageOffset_(vaddr); // Add the page offset

                translation_state->setResult(paddr, request.getSize());
                ILOG("  Result: " << HEX(paddr, width));

                // Keep going
                return nullptr;
            }
            // Read next level PTE
            else
            {
                ppn = pte.getPpn() << PAGESHIFT;
            }
            --level;
        }

        // If we got here, then Atlas could not translate the address
        // at any level.  We throw at this point.
        switch (TYPE)
        {
            case AccessType::INSTRUCTION:
                translation_state->clearRequest();
                THROW_FETCH_PAGE_FAULT;
            case AccessType::STORE:
                THROW_STORE_AMO_PAGE_FAULT;
            case AccessType::LOAD:
                THROW_LOAD_PAGE_FAULT;
        }
    }

    // Being pedantic
    template ActionGroup*
    Translate::translate_<RV32, MMUMode::BAREMETAL, Translate::AccessType::INSTRUCTION>(
        AtlasState*);
    template ActionGroup*
    Translate::translate_<RV32, MMUMode::BAREMETAL, Translate::AccessType::LOAD>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV32, MMUMode::BAREMETAL, Translate::AccessType::STORE>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV32, MMUMode::SV32, Translate::AccessType::INSTRUCTION>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV32, MMUMode::SV32, Translate::AccessType::LOAD>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV32, MMUMode::SV32, Translate::AccessType::STORE>(AtlasState*);

    template ActionGroup*
    Translate::translate_<RV64, MMUMode::BAREMETAL, Translate::AccessType::INSTRUCTION>(
        AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::BAREMETAL, Translate::AccessType::LOAD>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::BAREMETAL, Translate::AccessType::STORE>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV32, Translate::AccessType::INSTRUCTION>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV32, Translate::AccessType::LOAD>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV32, Translate::AccessType::STORE>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV39, Translate::AccessType::INSTRUCTION>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV39, Translate::AccessType::LOAD>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV39, Translate::AccessType::STORE>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV48, Translate::AccessType::INSTRUCTION>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV48, Translate::AccessType::LOAD>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV48, Translate::AccessType::STORE>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV57, Translate::AccessType::INSTRUCTION>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV57, Translate::AccessType::LOAD>(AtlasState*);
    template ActionGroup*
    Translate::translate_<RV64, MMUMode::SV57, Translate::AccessType::STORE>(AtlasState*);

} // namespace atlas
