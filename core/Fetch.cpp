#include "core/Fetch.hpp"
#include "core/PegasusAllocatorWrapper.hpp"
#include "core/PegasusCore.hpp"
#include "core/Execute.hpp"
#include "core/translate/Translate.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{
    // Added execution_cache parameter to FetchParameters and Fetch constructor.
    // Allows for toggling on and off the exection cache
    Fetch::Fetch(sparta::TreeNode* fetch_node, const FetchParameters* p) :
        sparta::Unit(fetch_node),
        enable_execution_cache_(p->enable_execution_cache)
    {
        Action fetch_action =
            pegasus::Action::createAction<&Fetch::fetch_>(this, "fetch", ActionTags::FETCH_TAG);
        fetch_action_group_.addAction(fetch_action);

        // Action for dispatching fetch that has already been translated to an ExecutionPage.
        // Used when fetch hits in the execution cache and can skip the normal TL action and go straight to Execute.
        Action dispatch_action =
            pegasus::Action::createAction<&Fetch::dispatchTranslatedFetch_>(this,
                                                                            "dispatch_translated_fetch");
        translated_fetch_dispatch_action_group_.addAction(dispatch_action);

        Action decode_action =
            pegasus::Action::createAction<&Fetch::decode_>(this, "decode", ActionTags::DECODE_TAG);
        decode_action_group_.addAction(decode_action);
    }

    // For flushing the execution cache
    void Fetch::flushExecutionCache()
    {
        if (execution_pages_.empty())
        {
            return;
        }

        execution_pages_.clear();
    }

    void Fetch::onBindTreeEarly_()
    {
        auto hart_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
        state_ = hart_tn->getResourceAs<PegasusState>();

        // Connect Fetch, Translate and Execute
        Translate* translate_unit = hart_tn->getChild("translate")->getResourceAs<Translate*>();
        Execute* execute_unit = hart_tn->getChild("execute")->getResourceAs<Execute*>();

        ActionGroup* inst_translate_action_group = translate_unit->getExecuteTranslateActionGroup();
        execute_action_group_ = execute_unit->getActionGroup();

        fetch_action_group_.setNextActionGroup(inst_translate_action_group);
        // If execution cache enabled, then after TL, dispatch to either execution cache check or decode
        inst_translate_action_group->setNextActionGroup(enable_execution_cache_
                                                            ? &translated_fetch_dispatch_action_group_
                                                            : &decode_action_group_);
        // Defaults to decode, but if fetch hits, it'll redirect
        translated_fetch_dispatch_action_group_.setNextActionGroup(&decode_action_group_);
        decode_action_group_.setNextActionGroup(execute_action_group_);
        execute_action_group_->setNextActionGroup(&fetch_action_group_);
    }

    Action::ItrType Fetch::fetch_(PegasusState* state, Action::ItrType action_it)
    {
        ILOG("Fetching PC 0x" << std::hex << state->getPc());

        if (state->consumeExecutionCacheFlushRequest())
        {
            flushExecutionCache();
        }

        // Reset the sim state
        PegasusState::SimState* sim_state = state->getSimState();
        // Preserve partial_opcode and current_opcode across reentry: a cross-page instruction
        // threw back to fetch_ to translate the second page.  current_opcode holds the first
        // 16 bits that setupInst_ stored before throwing; the second page's setupInst_ will
        // OR in the upper 16 bits, so both values must survive reset().
        const bool was_partial = sim_state->partial_opcode;
        const Opcode saved_opcode = sim_state->current_opcode;
        sim_state->reset();
        sim_state->partial_opcode = was_partial;
        if (was_partial) {
            sim_state->current_opcode = saved_opcode;
        }

        PegasusTranslationState* translation_state = state->getFetchTranslationState();
        // Reset the translation state and make a new translation request for the current PC.  This will be consumed by Translate and used to determine the fetch address and size.
        translation_state->reset();
        translation_state->makeRequest(state->getPc(), sizeof(Opcode));

        // Keep going
        return ++action_it;
    }

    // New in-between Action for determining whether to dispatch to an ExecutionPage from the cache or go to the normal decode
    Action::ItrType Fetch::dispatchTranslatedFetch_(PegasusState* state, Action::ItrType action_it)
    {
        // Obtain translation results from the translation state.
        auto* translation_state = state->getFetchTranslationState();
        sparta_assert(translation_state->getNumResults() > 0);

        const auto & result = translation_state->getResult();
        const Addr page_size = result.getPageSize();
        // If the translation result doesn't have a valid page size or it's less than 4K
        // We can't use the execution cache, so just dispatch it to decode
        if ((page_size < 0x1000) || ((page_size & (page_size - 1)) != 0))
        {
            translated_fetch_dispatch_action_group_.setNextActionGroup(&decode_action_group_);
            return ++action_it;
        }

        const Addr page_mask = page_size - 1;
        // Calculate the base virtual and phys addrs for the corresponding execution page
        // TO DO: does this every time, maybe add a page pointer so need to do it less?
        const Addr vaddr_base = result.getVAddr() & ~page_mask;
        const Addr paddr_base = result.getPAddr() & ~page_mask;
        const ExecutionPageKey key{vaddr_base, paddr_base, page_size};

        // If we don't yet have an execution page for this TL result, create one and add it to the map.
        auto [it, inserted] = execution_pages_.try_emplace(key, nullptr);
        if(inserted || !it->second) {
            // Can disable this ILOG if it disrupts test output
            ILOG("Creating new execution page for vaddr 0x" << std::hex << vaddr_base
                 << " paddr 0x" << paddr_base << " size 0x" << page_size);
            it->second = std::make_unique<ExecutionPage>(result,
                                                         &fetch_action_group_,
                                                         execute_action_group_);
        }

        translation_state->popResult();
        translated_fetch_dispatch_action_group_.setNextActionGroup(
            // Set next action group to the execution page's action group for dispatching translated fetches
            // It executes translatedPageExecute, which by default executes setupInst when an inst is first seen
            // Otherwise, it executes the instruction's action group after setupInst has been executed once, skipping decode
            it->second->getExecutionPageActionGroup());
        return ++action_it;
    }

    Action::ItrType Fetch::decode_(PegasusState* state, Action::ItrType action_it)
    {
        // Get translation result
        const PegasusTranslationState::TranslationResult result =
            state->getFetchTranslationState()->getResult();
        state->getFetchTranslationState()->popResult();

        // When compressed instructions are enabled, it is possible for a full sized instruction (32
        // bits) to cross a 4K page boundary meaning that first 16 bits of the instruction are on a
        // different page than the second 16 bits. Fetch will always request translation for a 32
        // bit memory access but Translate may need to be performed twice if it detects that the
        // access crosses a 4K page boundary. Since it is possible for the first 16 bits translated
        // and read from memory to result in a valid compressed instruction, Decode must attempt to
        // decode the first 16 bits before asking Translate to translate the second 16 bit access.
        // This ensures that the correct translation faults are triggered.
        //
        // There are several possible scenarios that result in Decode generating a valid
        // instruction:
        //
        // 1. The 32 bit fetch access does not cross a page boundary. The 32 bits read from memory
        // are
        //    decoded as a non-compressed instruction.
        //
        // 2. The 32 bit fetch access does not cross a page boundary. The 32 bits read from memory
        // are
        //    decoded as a compressed instruction. The extra 16 bits are discarded.
        //
        // 3. The 32 bit fetch access crosses a page boundary. The first 16 bits read are a
        // compressed
        //    instruction. The second 16 bits are never translated or read from memory.
        //
        // 4. The 32 bit fetch access crosses a page boundary. The first 16 bits read are not a
        // valid
        //    compressed instruction. The second 16 bits are translated and read from memory. The
        //    combined 32 bits are decoded as a non-compressed instruction.
        const bool page_crossing_access = result.getSize() == 2;

        // Read opcode from memory
        Opcode & opcode = state->getSimState()->current_opcode;
        OpcodeSize opcode_size = 4;
        if (SPARTA_EXPECT_TRUE(!page_crossing_access))
        {
            std::vector<uint8_t> buffer;
            if (state->readMemory<uint32_t>(result, buffer, MemAccessSource::FETCH) == false)
            {
                THROW_FETCH_ACCESS;
            }
            opcode = convertFromByteVector<uint32_t>(buffer);

            // Compression detection
            if ((opcode & 0x3) != 0x3)
            {
                opcode = opcode & 0xFFFF;
                opcode_size = 2;
            }
        }
        else
        {
            if (opcode == 0)
            {
                // Load the first 2B, could be a valid 2B compressed inst
                std::vector<uint8_t> buffer;
                if (state->readMemory<uint16_t>(result, buffer, MemAccessSource::FETCH) == false)
                {
                    THROW_FETCH_ACCESS;
                }
                opcode = convertFromByteVector<uint16_t>(buffer);
                opcode_size = 2;

                if ((opcode & 0x3) == 0x3)
                {
                    // Go back to inst translate
                    throw ActionException(fetch_action_group_.getNextActionGroup());
                }
            }
            else
            {
                // Load the second 2B of a possible 4B inst
                std::vector<uint8_t> buffer;
                if (state->readMemory<uint16_t>(result, buffer, MemAccessSource::FETCH) == false)
                {
                    THROW_FETCH_ACCESS;
                }
                opcode |= convertFromByteVector<uint16_t>(buffer) << 16;
            }
        }

        // Decode instruction with Mavis
        PegasusInstPtr inst = nullptr;
        try
        {
            inst = state->getMavis()->makeInst(opcode, state);
            inst->updateVectorConfig(state); // Old PegasusInst may be returned from cache. So
                                             // outside-constructor call is needed.
            assert(state->getCurrentInst() == nullptr);
            state->setCurrentInst(inst);
            // Set next PC, can be overidden by a branch/jump instruction or an exception
            state->setNextPc(state->getPc() + opcode_size);
        }
        catch (const mavis::BaseException & e)
        {
            THROW_ILLEGAL_INST;
        }

        // If we only fetched 2B and found a valid compressed inst, then cancel the translation
        // request for the second 2B
        if (page_crossing_access && (opcode_size == 2))
        {
            state->getFetchTranslationState()->popRequest();
        }

        // Check if Zvfh/Zvfhmin are enabled for vector BF16 support
        if (SPARTA_EXPECT_FALSE(inst->isVector() && inst->isFloat()
                                && (state->getVectorConfig()->getSEW() == 16)))
        {
            if (false == state->isExtensionEnabled("zfh"))
            {
                if (false == (state->isExtensionEnabled("zfhmin") && inst->hasMavisTag("zfhmin")))
                {
                    THROW_ILLEGAL_INST;
                }
            }
        }

        // FIXME: This is probably not the best place for these checks
        if (SPARTA_EXPECT_FALSE(inst->hasCsr()))
        {
            const uint32_t csr =
                inst->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
            if (state->getCsrRegister(csr) == nullptr)
            {
                THROW_ILLEGAL_INST;
            }

            if (csr == SATP)
            {
                const uint32_t tvm_val = READ_CSR_FIELD<RV64>(state, MSTATUS, "tvm");
                if ((state->getPrivMode() == PrivMode::SUPERVISOR) && tvm_val)
                {
                    THROW_ILLEGAL_INST;
                }
            }
        }

        return ++action_it;
    }
} // namespace pegasus
