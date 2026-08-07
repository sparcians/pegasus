#include "ExecutionPage.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusCore.hpp"
#include "core/translate/Translate.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    namespace
    {
        // Keep exec-cache tap usable without generating multi-GB logs, bc things get long otherwise
        constexpr uint64_t kExecCacheReuseLogSampleStride = 100000;
    } // namespace

    //
    // This method does the following:
    //
    // * For the current PC (vaddr), does that address fall within
    //   this page?
    // * If so, dissect the address and look up the InstExecute object
    //   to execute the instruction
    // * If the instruction has never been seen before,
    //   InstExecute::setupInst_ will be called
    // * If the instruction has been seen before, the instruction's
    //   handler will be called directly as the return action group
    //
    // To find the decoded block that holds preprocessed instructions,
    // the address is dissected into two parts: index into a map and
    // an offset into a vector:
    //
    //    InstExecute & obj = decoded_inst_map_[addr_idx][offset]
    //
    // The addr_idx is calculated by differencing the offset from the
    // page size.  This will produce an index range between 0 for a 4k
    // page and 0x3fffffff for 256T page.  Making a vector for large
    // pages is impractical especially if the program only uses a
    // fraction of that memory, so use an unordered_map.
    //
    // The offset is smallest page supported by RV is 4k (2^12).  This
    // value can be used as the offset into a simple vector.  However,
    // the vector does not need to be a 4k in size since instructions
    // are always aligned (at minimum) to a 2 byte address.
    // Therefore, the offset can be shifted to the right by 1 to
    // reduce memory usage (X is "don't care").
    //
    // +--------------------------------------------------------+------------+-+
    // |                  |                                     |11|         | |
    // |     xlation      |         page index                  |10|987654321|0|
    // +--------------------------------------------------------+------------+-+
    // |   va->pa bits    |    addr_idx (based on size)         |   offset   |X|
    // +--------------------------------------------------------+------------+-+
    //
    // One of the issues with using 4k page offset is when an half an
    // opcode is on page 1 and the other half is one page 2.  This
    // only happens at the 4k address 0xffe.  At this point, the
    // instruction at 0xffe is either a valid 16-bit opcode (which
    // nothing happens) or is invalid and the next 16-bit are needed
    // to decode.
    //
    // The second half of the opcode can be on a totally different
    // page.  The design here is to grab the first 16-bits and store
    // them in the global opcode in PegasusState's SimState structure.
    // Then, request a translation for the next page.  That next page
    // will check for partial opcode construction and read 16-bits to
    // build the complete opcode.  Then, the _second_ page will own
    // and execute that instruction.
    //
    //
    Action::ItrType ExecutionPage::translatedPageExecute_(PegasusState* state,
                                                          Action::ItrType action_it)
    {
        state->recordExecCacheBypassEnter();

        // Handle deferred flushes on the exec-page path and fall back to fetch.
        if (SPARTA_EXPECT_FALSE(state->consumeExecutionCacheFlushRequest()))
        {
            state->recordExecCacheBypassFallback();
            state->getTranslateUnit()->flushExecutionCache();
            state->getNextCycleResetActionGroup()->setNextActionGroup(fetch_action_group_);
            translated_page_group_.setNextActionGroup(fetch_action_group_);
            return ++action_it;
        }

        // Check to see if the PC is still within the page covered by this ExecutionPage.
        // isContained method no longer exists so the page mask and base addrs are calculated here.

        const Addr page_size = translation_result_.getPageSize() != 0
                                   ? translation_result_.getPageSize()
                                   : translation_result_.getSize();
        sparta_assert(page_size >= 0x1000 && ((page_size & (page_size - 1)) == 0),
                      "ExecutionPage requires power-of-two page size >= 4K, got " << page_size);
        const Addr page_mask = page_size - 1;
        const Addr vaddr_base = translation_result_.getVAddr() & ~page_mask;
        const Addr paddr_base = translation_result_.getPAddr() & ~page_mask;

        // Check to see if we're still on the same page
        const auto vaddr = state->getPc();
        if ((vaddr & ~page_mask) == vaddr_base)
        {
            // Wire Next Cycle Reset to loop back through this page. If PC leaves
            // this page, translatedPageExecute_ will redirect NCR to fetch on the next entry.
            state->getNextCycleResetActionGroup()->setNextActionGroup(&translated_page_group_);

            // std::cout << "Contained: " << std::hex << vaddr << std::endl;

            // Get the address index and shift out the offset
            const auto addr_idx = (vaddr - vaddr_base) >> 12;

            // Get the offset (lower 4k) and shift by 1 since that bit
            // is never set (and we can save some vector space)
            const auto offset = (vaddr & 0xfffull) >> 1;

            auto inst_execute_pair = decode_block_.try_emplace(addr_idx, default_block_).first;

            auto & inst_execute = inst_execute_pair->second.at(offset);

            // Logging stuff
            // For logging purposes, check if the inst has been decoded before (already set up)
            const bool instruction_reuse = inst_execute.isDecoded();

            // Track reuse rate for end-of-run summary.
            state->recordExecCacheDecision(instruction_reuse);

            inst_execute.setInstAddress(paddr_base + (vaddr - vaddr_base));

            // Log only sampled reuse events in stride to avoid per-instruction I/O in this hot
            // path. Stride is set such that we log the first reuse event and then every
            // kExecCacheReuseLogSampleStride reuse events after that.
            auto & exec_cache_logger = state->getExecCacheLogger();
            if (exec_cache_logger && instruction_reuse)
            {
                const uint64_t reuse_count = state->getExecCacheReuseCount();
                if ((reuse_count == 1) || ((reuse_count % kExecCacheReuseLogSampleStride) == 0))
                {
                    exec_cache_logger << "Exec-cache reuse sample at PC 0x" << std::hex << vaddr
                                      << std::dec << " reuse_count=" << reuse_count;
                }
            }

            // If this instruction was never fetched/decoded, the
            // instruction group being called is the Decode action
            // group.  This group returns the execution group
            translated_page_group_.setNextActionGroup(inst_execute.getInstActionGroup());
        }
        else
        {
            // Go back to translate
            state->recordExecCacheBypassFallback();
            // Translation request needed
            auto* fetch_translation_state = state->getFetchTranslationState();
            fetch_translation_state->reset();
            fetch_translation_state->makeRequest(vaddr, sizeof(Opcode));

            auto* translate_group = state->getFetchUnit()->getActionGroup()->getNextActionGroup();
            translated_page_group_.setNextActionGroup(translate_group);
        }

        // The translated page cannot continue.
        return ++action_it;
    }

    Action::ItrType ExecutionPage::InstExecute::setInst_(PegasusState* state,
                                                         Action::ItrType action_it)
    {
        // Set the current instruction
        state->setCurrentInst(inst_);

        // Reused instruction objects must refresh dynamic vector configuration
        // (VL/VTYPE/SEW/LMUL) from architectural state on each execution.
        inst_->updateVectorConfig(state);

        // Reuse path bypasses decode, so refresh sim-state opcode fields explicitly.
        auto* sim_state = state->getSimState();
        sim_state->current_opcode = static_cast<Opcode>(inst_->getOpcode());

        // Cross-page reuse: partial_opcode was preserved by fetch_() across reset() when
        // the first page went back to fetch for the second page.  The PC currently in
        // state is 2 bytes into the second page (where the second half was read), but the
        // logical PC of the instruction is 2 bytes earlier (on the first page).
        if (SPARTA_EXPECT_FALSE(sim_state->partial_opcode))
        {
            state->setPc(state->getPc() - 2);
            sim_state->partial_opcode = false;
        }

        // Assume we're heading to the next instruction in sequence.
        // Branches will adjust this.
        state->setNextPc(state->getPc() + inst_->getOpcodeSize());

        // Route the next action_group_ after the finish group (NextCycleReset)
        ActionGroup* next_action_group = bypass_action_group_;
        // Non-control flow or end-of-page instructions go to the next inst_execute's group
        if (bypass_inst_execute_ != nullptr)
        {
            next_action_group = bypass_inst_execute_->getInstActionGroup();
        }

        if (next_action_group != nullptr)
        {
            state->getNextCycleResetActionGroup()->setNextActionGroup(next_action_group);
        }

        return ++action_it;
    }

    // Need to decode the instruction at the offset
    Action::ItrType ExecutionPage::InstExecute::setupInst_(PegasusState* state,
                                                           Action::ItrType action_it)
    {
        // Decode the instruction at the given PC (in PegasusState)

        // When compressed instructions are enabled, it is possible
        // for a full sized instruction (32 bits) to cross a 4K page
        // boundary meaning that first 16 bits of the instruction are
        // on a different page than the second 16 bits. Fetch will
        // always request translation for a 32 bit memory access but
        // Translate may need to be performed twice if it detects that
        // the access crosses a 4K page boundary. Since it is possible
        // for the first 16 bits translated and read from memory to
        // result in a valid compressed instruction, Decode must
        // attempt to decode the first 16 bits before asking Translate
        // to translate the second 16 bit access.  This ensures that
        // the correct translation faults are triggered.
        //
        // There are several possible scenarios that result in Decode
        // generating a valid instruction:
        //
        // 1. The 32 bit fetch access does not cross a page
        //    boundary. The 32 bits read from memory are decoded as a
        //    non-compressed instruction.
        //
        // 2. The 32 bit fetch access does not cross a page
        //    boundary. The 32 bits read from memory are decoded as a
        //    compressed instruction. The extra 16 bits are discarded.
        //
        // 3. The 32 bit fetch access crosses a page boundary. The
        //    first 16 bits read are a compressed instruction. The
        //    second 16 bits are never translated or read from memory.
        //
        // 4. The 32 bit fetch access crosses a page boundary. The
        //    first 16 bits read are not a valid compressed
        //    instruction. The second 16 bits are translated and read
        //    from memory. The combined 32 bits are decoded as a
        //    non-compressed instruction.
        //

        auto sim_state = state->getSimState();
        bool & partial_opcode = sim_state->partial_opcode;

        // Point to the current opcode in state
        Opcode & opcode = sim_state->current_opcode;

        // Default size
        OpcodeSize opcode_size = 4;

        // If this is not the last entry in the page, then we know we can fetch 4 bytes and decode.
        // If it is the last entry, then we need to check for partial opcode construction.
        if (SPARTA_EXPECT_TRUE(false == last_entry_))
        {
            // Check to see if we're building an opcode from a
            // previous page.  If so, merge in those missing 16 bits
            if (SPARTA_EXPECT_FALSE(partial_opcode))
            {
                // Grab 2 bytes from the new page and merge with the old to create the
                // full opcode
                std::vector<uint8_t> buffer;
                if (state->readMemory<uint16_t>(inst_addr_, buffer, MemAccessSource::FETCH)
                    == false)
                {
                    THROW_FETCH_ACCESS;
                }
                opcode |= convertFromByteVector<uint16_t>(buffer) << 16;
                partial_opcode = false;
                // need to adjust the PC back since the first half of the opcode was on
                // the previous page and the second half is on this page.
                state->setPc(state->getPc() - 2);
            }
            else
            {
                // Grab 4 bytes
                std::vector<uint8_t> buffer;
                if (state->readMemory<uint32_t>(inst_addr_, buffer, MemAccessSource::FETCH)
                    == false)
                {
                    THROW_FETCH_ACCESS;
                }
                opcode = convertFromByteVector<uint32_t>(buffer);
            }
            // std::cout << std::hex << inst_addr_  << " opcode: " << opcode << std::endl;
        }
        else
        {

            // This is a fetch that is 2 bytes from the end of the page.
            // Two sub-cases:
            //
            //   The 2 bytes ARE a valid compressed instruction.
            //   Decode it here (don't go to the next page automatically)
            //
            //   The 2 bytes are the upper half of a 32-bit instruction
            //   that straddles the page boundary.  Store them in current_opcode,
            //   advance PC past the half-word already consumed, and throw back to
            //   fetch so the second page can supply the lower 16 bits and execute.
            std::vector<uint8_t> buffer;
            if (state->readMemory<uint16_t>(inst_addr_, buffer, MemAccessSource::FETCH) == false)
            {
                THROW_FETCH_ACCESS;
            }
            opcode = convertFromByteVector<uint16_t>(buffer);

            // If it's not a compressed instruction, then it's a partial opcode
            if (!((opcode & 0x3) != 0x3))
            {
                // Scenario 4: first half of a 32-bit cross-page instruction.
                partial_opcode = true;
                state->setPc(state->getPc() + 2);

                // Request translation for the second half directly and jump to
                // translate, avoiding an intermediate fetch_() reset path.
                auto* fetch_translation_state = state->getFetchTranslationState();
                fetch_translation_state->reset();
                fetch_translation_state->makeRequest(state->getPc(), sizeof(uint16_t));

                auto* translate_group =
                    state->getFetchUnit()->getActionGroup()->getNextActionGroup();
                throw ActionException(translate_group);
            }
        }

        // Compression detection
        if ((opcode & 0x3) != 0x3)
        {
            opcode = opcode & 0xFFFF;
            opcode_size = 2;
        }

        ++(state->getSimState()->current_uid);

        // Decode instruction with Mavis
        PegasusInstPtr inst = nullptr;
        try
        {
            inst = state->getMavis()->makeInst(opcode, state);
            // assert(state->getCurrentInst() == nullptr);
            state->setCurrentInst(inst);
            // Set next PC, can be overidden by a branch/jump
            // instruction or an exception
            state->setNextPc(state->getPc() + opcode_size);

            inst->updateVectorConfig(state); // Old PegasusInst may be returned from cache. So
                                             // outside-constructor call is needed.
        }
        catch (const mavis::BaseException & e)
        {
            THROW_ILLEGAL_INST;
        }

        if (SPARTA_EXPECT_FALSE(inst->hasCsr()))
        {
            const uint32_t csr =
                inst->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);
            if (state->getCsrRegister(csr) == nullptr)
            {
                THROW_ILLEGAL_INST;
            }

            // TODO: This is probably not the best place for this check...
            if (csr == SATP)
            {
                if ((state->getPrivMode() == PrivMode::SUPERVISOR) && tvm_val)
                {
                    auto tvm_val = READ_CSR_FIELD<RV64>(state, MSTATUS, "tvm");
                    if (tvm_val)
                    {
                        THROW_ILLEGAL_INST;
                    }
                }
            }
        }

        // translatedPageExecute Bypass:
        // If the instruction is guaranteed to remain within the page i.e not control flow
        // or page boundary, then we can bypass translatedPageExecute, which performs the
        // page checks, and instead directly wire this inst's actiongroup to the next for
        // future encounters.

        // Compute this slot's next target once; setInst_ publishes it to NCR
        // on each execution of the slot.
        bypass_action_group_ = nullptr;
        bypass_inst_execute_ = nullptr;

        // Control flow instructions route back to translated Page Execute in the event that
        // branches remain on the same page (not taken or small jumps)
        if (inst->isChangeOfFlowInst())
        {
            bypass_action_group_ = translated_page_group_;
        }
        else if ((true == last_entry_) || (true == partial_opcode))
        {
            // End-of-page / partial-opcode cases should re-enter translation on
            // the next cycle rather than attempting direct in-page bypass.
            // TODO: last_entry_ is only the end of a page in 4K size pages, it is NOT the end of
            // larger page sizes (default_block_ last entry)
            auto* fetch_translation_state = state->getFetchTranslationState();
            fetch_translation_state->reset();
            fetch_translation_state->makeRequest(state->getNextPc(), sizeof(Opcode));

            bypass_action_group_ = state->getFetchUnit()->getActionGroup()->getNextActionGroup();
        }
        else
        {
            // Essentially what translatedPageExecute does, but directly links instructions
            sparta_assert(owner_ != nullptr, "InstExecute owner_ should never be null");
            const Addr next_pc = state->getNextPc();
            const Addr page_size = owner_->translation_result_.getPageSize() != 0
                                       ? owner_->translation_result_.getPageSize()
                                       : owner_->translation_result_.getSize();
            const Addr page_mask = page_size - 1;
            const Addr vaddr_base = owner_->translation_result_.getVAddr() & ~page_mask;
            const Addr paddr_base = owner_->translation_result_.getPAddr() & ~page_mask;

            const auto addr_idx = (next_pc - vaddr_base) >> 12;

            // Get the offset (lower 4k) and shift by 1 since that bit
            // is never set (and we can save some vector space)
            const auto offset = (next_pc & 0xfffull) >> 1;

            auto next_inst_execute_pair =
                owner_->decode_block_.try_emplace(addr_idx, owner_->default_block_).first;
            auto & next_inst_execute = next_inst_execute_pair->second.at(offset);
            state->recordExecCachePteBypassSetup();

            next_inst_execute.setInstAddress(paddr_base + (next_pc - vaddr_base));

            // inst_execute is stored so routing doesn't go to stale groups
            bypass_inst_execute_ = &next_inst_execute;
        }

        // Set up the execution of the instruction and reset the
        // instruction execute group to the instruction that was just
        // setup
        inst_action_group_ = execute_action_group_->execute(state);

        // Before the instruction's execution PegasusState needs to be
        // set to the captured instruction.
        inst_action_group_->insertActionFront(inst_set_inst_);

        // Capture the instruction late -- the above execute functions
        // can throw
        inst_ = state->getCurrentInst();

        // Set the next action group to the inst_action_group for the return!
        inst_setup_group_.setNextActionGroup(inst_action_group_);

        // Go to end...
        return ++action_it;
    }
} // namespace pegasus
