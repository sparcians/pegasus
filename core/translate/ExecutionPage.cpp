#include "ExecutionPage.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusCore.hpp"

namespace pegasus
{

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
        // Check to see if we're still on the same page
        const auto vaddr = state->getPc();
        if (translation_result_.isContained(vaddr))
        {
            //std::cout << "Contained: " << std::hex << vaddr << std::endl;

            // Get the address index and shift out the offset
            const auto addr_idx = translation_result_.genAddrIndx(vaddr) >> 12;

            // Get the offset (lower 4k) and shift by 1 since that bit
            // is never set (and we can save some vector space)
            const auto offset = (vaddr & 0xfffull) >> 1;

            auto inst_execute_pair = decode_block_.try_emplace(addr_idx, default_block_).first;

            auto & inst_execute = inst_execute_pair->second.at(offset);

            inst_execute.setInstAddress(translation_result_.genPAddr(vaddr));
            // If this instruction was never fetched/decoded, the
            // instruction group being called is the Decode action
            // group.  This group returns the execution group
            translated_page_group_.
                setNextActionGroup(inst_execute.getInstActionGroup());
        }
        else {
            // Go back to fetch
            translated_page_group_.setNextActionGroup(fetch_action_group_);
        }

        // The translated page cannot continue.
        return ++action_it;
    }

    Action::ItrType ExecutionPage::InstExecute::setInst_(PegasusState* state,
                                                         Action::ItrType action_it)
    {
        // Set the current instruction
        state->setCurrentInst(inst_);

        // Assume we're heading to the next instruction in sequence.
        // Branches will adjust this.
        state->setNextPc(state->getPc() + inst_->getOpcodeSize());
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

        if (SPARTA_EXPECT_TRUE(false == last_entry_))
        {
            // Check to see if we're building an opcode from a
            // previous page.  If so, merge in those missing 16 bits
            if (SPARTA_EXPECT_FALSE(partial_opcode))
            {
                opcode |= state->readMemory<uint16_t>(inst_addr_) << 16;
                partial_opcode = false;
            }
            else {
                // Grab 4 bytes
                opcode = state->readMemory<uint32_t>(inst_addr_);
            }
            //std::cout << std::hex << inst_addr_  << " opcode: " << opcode << std::endl;
        }
        else {

            // This is a fetch that is 2 bytes from the end of the
            // page.
            opcode = state->readMemory<uint16_t>(inst_addr_);
            partial_opcode = true;

            //std::cout << std::hex << inst_addr_  << " 16 bit read " << opcode << " partial throwing" << std::endl;

            state->setPc(inst_addr_ + 2);

            // Go back to inst translate to get the second page.  Have
            // that second page do the execution
            throw ActionException(translated_page_group_);
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
            inst = state->getCore()->getMavis()->makeInst(opcode, state);
            //assert(state->getCurrentInst() == nullptr);
            state->setCurrentInst(inst);
            // Set next PC, can be overidden by a branch/jump
            // instruction or an exception
            state->setNextPc(state->getPc() + opcode_size);

            inst->updateVecConfig(state); // Old PegasusInst may be returned from cache. So
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
                const uint32_t tvm_val = READ_CSR_FIELD<RV64>(state, MSTATUS, "tvm");
                if ((state->getPrivMode() == PrivMode::SUPERVISOR) && tvm_val)
                {
                    THROW_ILLEGAL_INST;
                }
            }
        }

        // Set up the execution of the instruction and reset the
        // instruction execute group to the instruction that was just
        // setup
        inst_action_group_ = execute_action_group_->execute(state);

        // Before the instruction's execution PegasusState needs to be
        // set to the captured instruction.
        inst_action_group_->insertActionFront(inst_set_inst_);

        // Setup the instruction's next action (after execute adds
        // finishing action groups) to return to this translated page
        inst_action_group_->getNextActionGroup()->
            setNextActionGroup(translated_page_group_);

        // Capture the instruction late -- the above execute functions
        // can throw
        inst_ = state->getCurrentInst();

        // Set the next action group to the inst_action_group for the return!
        inst_setup_group_.setNextActionGroup(inst_action_group_);

        // Go to end...
        return ++action_it;
    }
}
