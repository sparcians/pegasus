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
        params_(p),
        enable_ecache_(p->enable_ecache)
    {
        Action fetch_action =
            pegasus::Action::createAction<&Fetch::fetch_>(this, "fetch", ActionTags::FETCH_TAG);
        fetch_action_group_.addAction(fetch_action);

        Action decode_action =
            pegasus::Action::createAction<&Fetch::decode_>(this, "decode", ActionTags::DECODE_TAG);
        decode_action_group_.addAction(decode_action);
    }

    // For flushing the execution cache
    void Fetch::flushExecutionCache()
    {
        if (state_ != nullptr)
        {
            state_->getTranslateUnit()->flushExecutionCache();
        }
    }

    void Fetch::onBindTreeEarly_()
    {
        auto hart_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
        state_ = hart_tn->getResourceAs<PegasusState>();
        // enable_ecache_ is read in the constructor from params so it is available
        // before PegasusState::onBindTreeEarly_() queries it.

        // Connect Fetch, Translate and Execute
        Translate* translate_unit = hart_tn->getChild("translate")->getResourceAs<Translate*>();
        Execute* execute_unit = hart_tn->getChild("execute")->getResourceAs<Execute*>();

        ActionGroup* inst_translate_action_group = translate_unit->getExecuteTranslateActionGroup();
        execute_action_group_ = execute_unit->getActionGroup();

        fetch_action_group_.setNextActionGroup(inst_translate_action_group);
        inst_translate_action_group->setNextActionGroup(&decode_action_group_);
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
        // Reset sim state. When ecache is off this always runs. When ecache is on, NCR
        // handles the reset mid-stream (after NCR clears current_inst to null, the check
        // below is false so fetch_ is a no-op). On quantum/pause resume advanceSim_()
        // re-enters at fetch_action_group_ directly, bypassing NCR; current_inst is still
        // non-null from the last instruction, so the guard fires and resets correctly.
        if (!enable_ecache_ || state->getCurrentInst() != nullptr)
        {
            state->getSimState()->reset();
        }

        PegasusTranslationState* translation_state = state->getFetchTranslationState();
        // Reset the translation state and make a new translation request for the current PC.  This will be consumed by Translate and used to determine the fetch address and size.
        translation_state->reset();
        translation_state->makeRequest(state->getPc(), sizeof(Opcode));

        // Keep going
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
