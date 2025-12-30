
#include <vector>

#include "core/translate/Translate.hpp"
#include "core/translate/PageTableEntry.hpp"
#include "core/PegasusInst.hpp"
#include "core/PegasusState.hpp"

#include "include/ActionTags.hpp"

#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{

    Translate::Translate(sparta::TreeNode* translate_node, const TranslateParameters*) :
        sparta::Unit(translate_node)
    {
        // Baremetal (translation disabled)
        registerAction_<RV32, translate_types::TranslationMode::BAREMETAL,
                        translate_types::AccessType::EXECUTE>("Inst Translate (RV32 Baremetal)",
                                                              ActionTags::INST_TRANSLATE_TAG,
                                                              rv32_translation_actions_);
        registerAction_<RV32, translate_types::TranslationMode::BAREMETAL,
                        translate_types::AccessType::LOAD>("Load Translate (RV32 Baremetal)",
                                                           ActionTags::DATA_TRANSLATE_TAG,
                                                           rv32_translation_actions_);
        registerAction_<RV32, translate_types::TranslationMode::BAREMETAL,
                        translate_types::AccessType::STORE>("Store Translate (RV32 Baremetal)",
                                                            ActionTags::DATA_TRANSLATE_TAG,
                                                            rv32_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::BAREMETAL,
                        translate_types::AccessType::EXECUTE>("Inst Translate (Baremetal)",
                                                              ActionTags::INST_TRANSLATE_TAG,
                                                              rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::BAREMETAL,
                        translate_types::AccessType::LOAD>("Load Translate (Baremetal)",
                                                           ActionTags::DATA_TRANSLATE_TAG,
                                                           rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::BAREMETAL,
                        translate_types::AccessType::STORE>("Store Translate (Baremetal)",
                                                            ActionTags::DATA_TRANSLATE_TAG,
                                                            rv64_translation_actions_);

        // Sv32
        registerAction_<RV32, translate_types::TranslationMode::SV32,
                        translate_types::AccessType::EXECUTE>("Inst Translate (RV32 Sv32)",
                                                              ActionTags::INST_TRANSLATE_TAG,
                                                              rv32_translation_actions_);
        registerAction_<RV32, translate_types::TranslationMode::SV32,
                        translate_types::AccessType::LOAD>("Load Translate (RV32 Sv32)",
                                                           ActionTags::DATA_TRANSLATE_TAG,
                                                           rv32_translation_actions_);
        registerAction_<RV32, translate_types::TranslationMode::SV32,
                        translate_types::AccessType::STORE>("Store Translate (RV32 Sv32)",
                                                            ActionTags::DATA_TRANSLATE_TAG,
                                                            rv32_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::SV32,
                        translate_types::AccessType::EXECUTE>(
            "Inst Translate (Sv32)", ActionTags::INST_TRANSLATE_TAG, rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::SV32,
                        translate_types::AccessType::LOAD>(
            "Load Translate (Sv32)", ActionTags::DATA_TRANSLATE_TAG, rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::SV32,
                        translate_types::AccessType::STORE>(
            "Store Translate (Sv32)", ActionTags::DATA_TRANSLATE_TAG, rv64_translation_actions_);

        // Sv39
        registerAction_<RV64, translate_types::TranslationMode::SV39,
                        translate_types::AccessType::EXECUTE>(
            "Inst Translate (Sv39)", ActionTags::INST_TRANSLATE_TAG, rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::SV39,
                        translate_types::AccessType::LOAD>(
            "Load Translate (Sv39)", ActionTags::DATA_TRANSLATE_TAG, rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::SV39,
                        translate_types::AccessType::STORE>(
            "Store Translate (Sv39)", ActionTags::DATA_TRANSLATE_TAG, rv64_translation_actions_);

        // Sv48
        registerAction_<RV64, translate_types::TranslationMode::SV48,
                        translate_types::AccessType::EXECUTE>(
            "Inst Translate (Sv48)", ActionTags::INST_TRANSLATE_TAG, rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::SV48,
                        translate_types::AccessType::LOAD>(
            "Load Translate (Sv48)", ActionTags::DATA_TRANSLATE_TAG, rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::SV48,
                        translate_types::AccessType::STORE>(
            "Store Translate (Sv48)", ActionTags::DATA_TRANSLATE_TAG, rv64_translation_actions_);

        // Sv57
        registerAction_<RV64, translate_types::TranslationMode::SV57,
                        translate_types::AccessType::EXECUTE>(
            "Inst Translate (Sv57)", ActionTags::INST_TRANSLATE_TAG, rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::SV57,
                        translate_types::AccessType::LOAD>(
            "Load Translate (Sv57)", ActionTags::DATA_TRANSLATE_TAG, rv64_translation_actions_);
        registerAction_<RV64, translate_types::TranslationMode::SV57,
                        translate_types::AccessType::STORE>(
            "Store Translate (Sv57)", ActionTags::DATA_TRANSLATE_TAG, rv64_translation_actions_);

        // Assume we are booting in RV64 Machine mode with translation disabled
        execute_translate_action_group_.addAction(getTranslateAction_<RV64>(
            translate_types::AccessType::EXECUTE, translate_types::TranslationMode::BAREMETAL));
        load_translate_action_group_.addAction(getTranslateAction_<RV64>(
            translate_types::AccessType::LOAD, translate_types::TranslationMode::BAREMETAL));
        store_translate_action_group_.addAction(getTranslateAction_<RV64>(
            translate_types::AccessType::STORE, translate_types::TranslationMode::BAREMETAL));

        // Hypervisor
        rv32_hyp_translation_actions_[static_cast<uint32_t>(translate_types::AccessType::EXECUTE)] =
            pegasus::Action::createAction<
                &Translate::hypervisorTranslate_<RV32, translate_types::AccessType::EXECUTE>>(
                this, "Hypervisor Execute Translate", ActionTags::DATA_TRANSLATE_TAG);
        rv32_hyp_translation_actions_[static_cast<uint32_t>(translate_types::AccessType::LOAD)] =
            pegasus::Action::createAction<
                &Translate::hypervisorTranslate_<RV32, translate_types::AccessType::LOAD>>(
                this, "Hypervisor Load Translate", ActionTags::DATA_TRANSLATE_TAG);
        rv32_hyp_translation_actions_[static_cast<uint32_t>(translate_types::AccessType::STORE)] =
            pegasus::Action::createAction<
                &Translate::hypervisorTranslate_<RV32, translate_types::AccessType::STORE>>(
                this, "Hypervisor Store Translate", ActionTags::DATA_TRANSLATE_TAG);
        rv64_hyp_translation_actions_[static_cast<uint32_t>(translate_types::AccessType::EXECUTE)] =
            pegasus::Action::createAction<
                &Translate::hypervisorTranslate_<RV64, translate_types::AccessType::EXECUTE>>(
                this, "Hypervisor Execute Translate", ActionTags::DATA_TRANSLATE_TAG);
        rv64_hyp_translation_actions_[static_cast<uint32_t>(translate_types::AccessType::LOAD)] =
            pegasus::Action::createAction<
                &Translate::hypervisorTranslate_<RV64, translate_types::AccessType::LOAD>>(
                this, "Hypervisor Load Translate", ActionTags::DATA_TRANSLATE_TAG);
        rv64_hyp_translation_actions_[static_cast<uint32_t>(translate_types::AccessType::STORE)] =
            pegasus::Action::createAction<
                &Translate::hypervisorTranslate_<RV64, translate_types::AccessType::STORE>>(
                this, "Hypervisor Store Translate", ActionTags::DATA_TRANSLATE_TAG);

        // Assume we are booting in RV64
        hyp_execute_translate_action_group_.addAction(
            rv64_hyp_translation_actions_[static_cast<uint32_t>(
                translate_types::AccessType::EXECUTE)]);
        hyp_load_translate_action_group_.addAction(
            rv64_hyp_translation_actions_[static_cast<uint32_t>(
                translate_types::AccessType::LOAD)]);
        hyp_store_translate_action_group_.addAction(
            rv64_hyp_translation_actions_[static_cast<uint32_t>(
                translate_types::AccessType::STORE)]);
    }

    template <typename XLEN>
    void Translate::updateTranslationMode(const translate_types::TranslationStage type,
                                          const translate_types::TranslationMode mode,
                                          const translate_types::TranslationMode ls_mode)
    {
        sparta_assert(mode != translate_types::TranslationMode::INVALID);
        sparta_assert(ls_mode != translate_types::TranslationMode::INVALID);

        if (SPARTA_EXPECT_TRUE(type == translate_types::TranslationStage::SUPERVISOR))
        {
            if constexpr (std::is_same_v<XLEN, RV64>)
            {
                execute_translate_action_group_.replaceAction(
                    ActionTags::INST_TRANSLATE_TAG,
                    getTranslateAction_<XLEN>(translate_types::AccessType::EXECUTE, mode));
                load_translate_action_group_.replaceAction(
                    ActionTags::DATA_TRANSLATE_TAG,
                    getTranslateAction_<XLEN>(translate_types::AccessType::LOAD, ls_mode));
                store_translate_action_group_.replaceAction(
                    ActionTags::DATA_TRANSLATE_TAG,
                    getTranslateAction_<XLEN>(translate_types::AccessType::STORE, ls_mode));
            }
            else
            {
                execute_translate_action_group_.replaceAction(
                    ActionTags::INST_TRANSLATE_TAG,
                    getTranslateAction_<XLEN>(translate_types::AccessType::EXECUTE, mode));
                load_translate_action_group_.replaceAction(
                    ActionTags::DATA_TRANSLATE_TAG,
                    getTranslateAction_<XLEN>(translate_types::AccessType::LOAD, ls_mode));
                store_translate_action_group_.replaceAction(
                    ActionTags::DATA_TRANSLATE_TAG,
                    getTranslateAction_<XLEN>(translate_types::AccessType::STORE, ls_mode));
            }
        }
    }

    template void Translate::updateTranslationMode<RV32>(const translate_types::TranslationStage,
                                                         const translate_types::TranslationMode,
                                                         const translate_types::TranslationMode);
    template void Translate::updateTranslationMode<RV64>(const translate_types::TranslationStage,
                                                         const translate_types::TranslationMode,
                                                         const translate_types::TranslationMode);

    template <typename XLEN, translate_types::TranslationMode MODE,
              translate_types::AccessType TYPE>
    Action::ItrType Translate::translate_(PegasusState* state, Action::ItrType action_it)
    {
        PegasusTranslationState* translation_state = nullptr;
        if constexpr (TYPE == translate_types::AccessType::EXECUTE)
        {
            // Translation reqest is from fetch
            translation_state = state->getFetchTranslationState();
        }
        else
        {
            const auto & inst = state->getCurrentInst();
            translation_state = inst->getTranslationState();
        }

        // If there is no request, nothing to be done here
        if (!translation_state->getNumRequests())
        {
            return ++action_it;
        }

        // Get request from the request queue
        const PegasusTranslationState::TranslationRequest & request =
            translation_state->getRequest();
        const XLEN vaddr = request.isMisaligned()
                               ? (request.getVAddr() + request.getMisalignedBytes())
                               : request.getVAddr();

        // Width in bytes for logging
        const uint32_t width = std::is_same_v<XLEN, RV64> ? 16 : 8;
        ILOG("Translating " << HEX(vaddr, width));

        uint32_t level = translate_types::getNumPageWalkLevels<MODE>();
        const auto priv_mode = (TYPE == translate_types::AccessType::EXECUTE)
                                   ? state->getPrivMode()
                                   : state->getLdstPrivMode();

        // See if translation is disable -- no level walks
        if (level == 0 || (priv_mode == PrivMode::MACHINE))
        {
            return setResult_<XLEN, MODE, TYPE>(translation_state, action_it, vaddr);
        }

        // Smallest page size is 4K for both RV32 and RV64
        constexpr uint64_t PAGESHIFT = 12; // 4096
        // FIXME
        const uint32_t ATP_CSR = SATP;
        uint64_t ppn = READ_CSR_FIELD<XLEN>(state, ATP_CSR, "ppn") << PAGESHIFT;
        while (level > 0)
        {
            // Read PTE from memory
            const auto indexed_level = level - 1;
            const auto & vpn_field = translate_types::getVpnField<MODE>(indexed_level);
            const uint64_t pte_paddr = ppn + vpn_field.calcPTEOffset(vaddr) * sizeof(XLEN);
            PageTableEntry<XLEN, MODE> pte = state->readMemory<XLEN>(pte_paddr);
            DLOG_CODE_BLOCK(DLOG_OUTPUT("Level " << level << " Page Walk");
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
                const translate_types::PageSize page_size =
                    translate_types::getPageSize<MODE>(level);
                DLOG("    Size: " << page_size);

                // TODO: Check page alignment

                // If page is a super page, unused PPN fields should be zero
                if (page_size != translate_types::PageSize::SIZE_4K)
                {
                    bool throw_page_fault = false;
                    for (int32_t ppn_field_idx = indexed_level - 1; ppn_field_idx >= 0;
                         --ppn_field_idx)
                    {
                        const XLEN ppn_field = pte.getPpnField(ppn_field_idx);
                        if (ppn_field != 0)
                        {
                            DLOG("Translation FAILED! PPN field " << std::dec << ppn_field_idx
                                                                  << " is not zero: 0x" << std::hex
                                                                  << ppn_field);
                            throw_page_fault = true;
                        }
                    }
                    if (throw_page_fault)
                    {
                        break;
                    }
                }

                // If the SUM bit is set, Supervisor mode software is allowed to access User mode
                // pages
                const uint32_t sum_val = READ_CSR_FIELD<XLEN>(state, MSTATUS, "sum");
                if ((sum_val == 0) && (false == pte.isUserMode())
                    && (priv_mode != PrivMode::SUPERVISOR))
                {
                    DLOG("Translation FAILED! Cannot access User mode PTE");
                    break;
                }

                // Instruction (fetch) accesses must have execute permissions
                if ((TYPE == translate_types::AccessType::EXECUTE) && (false == pte.canExecute()))
                {
                    DLOG("Translation FAILED! PTE does not have execute access");
                    break;
                }

                // Load accesses must have read permissions
                if ((TYPE == translate_types::AccessType::LOAD) && (false == pte.canRead()))
                {
                    DLOG("Translation FAILED! PTE does not have read access");
                    break;
                }

                // Store accesses must have write permissions
                constexpr bool is_store = TYPE == translate_types::AccessType::STORE;
                if ((is_store) && (false == pte.canWrite()))
                {
                    DLOG("Translation FAILED! PTE does not have write access");
                    break;
                }

                if (false == pte.isAccessable(is_store))
                {
                    // See if we're required to update access bits in the PTE
                    if (READ_CSR_FIELD<XLEN>(state, MENVCFG, "adue"))
                    {
                        if constexpr (is_store)
                        {
                            pte.setDirty();
                            DLOG("Setting PTE dirty: " << pte);
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

                // Translate!
                const Addr index_bits = (vpn_field.msb - vpn_field.lsb + 1) * indexed_level;
                const Addr virt_base = vaddr >> PAGESHIFT;
                Addr paddr = (Addr(pte.getPpn()) | (virt_base & ((0b1 << index_bits) - 1)))
                             << PAGESHIFT;
                const Addr page_offset_mask =
                    translate_types::getPageOffsetMask<MODE>(indexed_level);
                paddr |= page_offset_mask & vaddr;

                // Set result and determine whether to keep going or performa translation again
                return setResult_<XLEN, MODE, TYPE>(translation_state, action_it, paddr, level);
            }
            // If PTE is NOT a leaf, keep walking the page table
            else
            {
                ppn = pte.getPpn() << PAGESHIFT;
            }

            // Go to next level
            --level;
        }

        // If we got here, then Pegasus could not translate the address
        // at any level.
        if (request.isNoThrow())
        {
            translation_state->clearRequest();
            return ++action_it;
        }
        // We throw at this point.
        switch (TYPE)
        {
            case translate_types::AccessType::EXECUTE:
                THROW_FETCH_PAGE_FAULT;
            case translate_types::AccessType::STORE:
                THROW_STORE_AMO_PAGE_FAULT;
            case translate_types::AccessType::LOAD:
                THROW_LOAD_PAGE_FAULT;
        }
    }

    template <typename XLEN, translate_types::TranslationMode MODE,
              translate_types::AccessType TYPE>
    Action::ItrType Translate::setResult_(PegasusTranslationState* translation_state,
                                          Action::ItrType action_it, const Addr paddr,
                                          const uint32_t level)
    {
        // Width in bytes for logging
        const uint32_t width = std::is_same_v<XLEN, RV64> ? 16 : 8;
        ILOG("   Result: " << HEX(paddr, width));

        PegasusTranslationState::TranslationRequest & request = translation_state->getRequest();
        const XLEN vaddr = request.isMisaligned()
                               ? (request.getVAddr() + request.getMisalignedBytes())
                               : request.getVAddr();
        const size_t access_size =
            request.isMisaligned() ? request.getMisalignedBytes() : request.getSize();

        // Check if address is misaligned
        const auto indexed_level = level - 1;
        const Addr page_offset_mask = translate_types::getPageOffsetMask<MODE>(indexed_level);
        const bool is_misaligned =
            ((vaddr & page_offset_mask) + access_size) > (page_offset_mask + 1);
        if (SPARTA_EXPECT_FALSE(is_misaligned))
        {
            sparta_assert(request.isMisaligned() == false);
            const size_t num_misaligned_bytes = (vaddr + access_size) % (page_offset_mask + 1);
            DLOG("Address is misaligned by " << std::dec << num_misaligned_bytes << "B!");

            // Resolve first request
            const size_t first_access_size = access_size - num_misaligned_bytes;
            translation_state->setResult(vaddr, paddr, first_access_size);

            // Set request as misaligned
            request.setMisaligned(num_misaligned_bytes);
        }
        else
        {
            translation_state->popRequest();
            translation_state->setResult(vaddr, paddr, access_size);
        }

        if (is_misaligned || (translation_state->getNumRequests() > 0))
        {
            // For misaligned accesses, there may be another translation request to resolve.
            // In some scenarios, Fetch/Decode may decide to not translate the second
            // so keep going and let Fetch/Decode determine if translation needs to be
            // performed again.
            switch (TYPE)
            {
                case translate_types::AccessType::EXECUTE:
                    sparta_assert(is_misaligned,
                                  "Should never receive multiple translation requests from Fetch!");
                    break;
                case translate_types::AccessType::STORE:
                case translate_types::AccessType::LOAD:
                    // Execute the translate Action again
                    return action_it;
            }
        }

        // Keep going
        return ++action_it;
    }

    // Being pedantic
    template Action::ItrType
    Translate::translate_<RV32, translate_types::TranslationMode::BAREMETAL,
                          translate_types::AccessType::EXECUTE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV32, translate_types::TranslationMode::BAREMETAL,
                          translate_types::AccessType::LOAD>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV32, translate_types::TranslationMode::BAREMETAL,
                          translate_types::AccessType::STORE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV32, translate_types::TranslationMode::SV32,
                          translate_types::AccessType::EXECUTE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV32, translate_types::TranslationMode::SV32,
                          translate_types::AccessType::LOAD>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV32, translate_types::TranslationMode::SV32,
                          translate_types::AccessType::STORE>(PegasusState*, Action::ItrType);

    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::BAREMETAL,
                          translate_types::AccessType::EXECUTE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::BAREMETAL,
                          translate_types::AccessType::LOAD>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::BAREMETAL,
                          translate_types::AccessType::STORE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV32,
                          translate_types::AccessType::EXECUTE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV32,
                          translate_types::AccessType::LOAD>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV32,
                          translate_types::AccessType::STORE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV39,
                          translate_types::AccessType::EXECUTE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV39,
                          translate_types::AccessType::LOAD>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV39,
                          translate_types::AccessType::STORE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV48,
                          translate_types::AccessType::EXECUTE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV48,
                          translate_types::AccessType::LOAD>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV48,
                          translate_types::AccessType::STORE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV57,
                          translate_types::AccessType::EXECUTE>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV57,
                          translate_types::AccessType::LOAD>(PegasusState*, Action::ItrType);
    template Action::ItrType
    Translate::translate_<RV64, translate_types::TranslationMode::SV57,
                          translate_types::AccessType::STORE>(PegasusState*, Action::ItrType);

    template <typename XLEN, translate_types::AccessType TYPE>
    Action::ItrType Translate::hypervisorTranslate_(PegasusState* state, Action::ItrType action_it)
    {
        const auto & inst = state->getCurrentInst();
        PegasusTranslationState* translation_state = inst->getTranslationState();

        // VS-stage translation
        const translate_types::TranslationMode vs_stage_mode = mmu_modes_[static_cast<uint32_t>(
            translate_types::TranslationStage::VIRTUAL_SUPERVISOR)];
        Action & translate_action = getTranslateAction_<XLEN>(TYPE, vs_stage_mode);
        Action::ItrType tmp_action_it;
        while (translation_state->getNumRequests() != 0)
        {
            translate_action.execute(state, tmp_action_it);
        }

        // Make new translation request
        std::vector<std::pair<Addr, size_t>> h_stage_requests;
        while (translation_state->getNumResults() != 0)
        {
            const PegasusTranslationState::TranslationResult & result =
                translation_state->getResult();
            h_stage_requests.emplace_back(result.getPAddr(), result.getSize());
            translation_state->popResult();
        }

        for (const auto & [addr, size] : h_stage_requests)
        {
            translation_state->makeRequest(addr, size);
        }

        // G-stage translation
        const translate_types::TranslationMode g_stage_mode =
            mmu_modes_[static_cast<uint32_t>(translate_types::TranslationStage::GUEST)];
        translate_action = getTranslateAction_<XLEN>(TYPE, g_stage_mode);
        while (translation_state->getNumRequests() != 0)
        {
            translate_action.execute(state, tmp_action_it);
        }

        return ++action_it;
    }
} // namespace pegasus
