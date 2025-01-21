#include "core/Fetch.hpp"
#include "core/AtlasAllocatorWrapper.hpp"
#include "core/AtlasState.hpp"
#include "core/Execute.hpp"
#include "core/Translate.hpp"
#include "include/ActionTags.hpp"

#include "sparta/events/StartupEvent.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace atlas
{
    Fetch::Fetch(sparta::TreeNode* fetch_node, const FetchParameters*) : sparta::Unit(fetch_node)
    {
        Action fetch_action =
            atlas::Action::createAction<&Fetch::fetch_>(this, "fetch", ActionTags::FETCH_TAG);
        fetch_action_group_.addAction(fetch_action);

        Action decode_action =
            atlas::Action::createAction<&Fetch::decode_>(this, "decode", ActionTags::DECODE_TAG);
        decode_action_group_.addAction(decode_action);

        sparta::StartupEvent(fetch_node, CREATE_SPARTA_HANDLER(Fetch, advanceSim_));
    }

    void Fetch::onBindTreeEarly_()
    {
        auto core_tn = getContainer()->getParentAs<sparta::ResourceTreeNode>();
        state_ = core_tn->getResourceAs<AtlasState>();

        // Connect Fetch, Translate and Execute
        Translate* translate_unit = core_tn->getChild("translate")->getResourceAs<Translate*>();
        Execute* execute_unit = core_tn->getChild("execute")->getResourceAs<Execute*>();

        ActionGroup* inst_translate_action_group = translate_unit->getInstTranslateActionGroup();
        ActionGroup* execute_action_group = execute_unit->getActionGroup();

        fetch_action_group_.setNextActionGroup(inst_translate_action_group);
        inst_translate_action_group->setNextActionGroup(&decode_action_group_);
        decode_action_group_.setNextActionGroup(execute_action_group);
        execute_action_group->setNextActionGroup(&fetch_action_group_);
    }

    ActionGroup* Fetch::fetch_(AtlasState* state)
    {
        ILOG("Fetching PC 0x" << std::hex << state->getPc());

        // Reset the sim state
        AtlasState::SimState* sim_state = state->getSimState();
        sim_state->reset();

        AtlasTranslationState* translation_state = state->getTranslationState();
        translation_state->makeTranslationRequest(state->getPc(), sizeof(Opcode));

        // Keep going
        return nullptr;
    }

    ActionGroup* Fetch::decode_(AtlasState* state)
    {
        // Get translation result
        const AtlasTranslationState* translation_state = state->getTranslationState();
        const AtlasTranslationState::TranslationResult result =
            translation_state->getTranslationResult();

        // Read opcode from memory
        std::vector<uint8_t> buffer(sizeof(result.size), 0);
        Opcode opcode = state->readMemory<Opcode>(result.physical_addr);

        // Compression detection
        OpcodeSize opcode_size = 4;
        if ((opcode & 0x3) != 0x3)
        {
            opcode = opcode & 0xFFFF;
            opcode_size = 2;
        }

        state->getSimState()->current_opcode = opcode;

        // Decode instruction with Mavis
        AtlasInstPtr inst = nullptr;
        try
        {
            inst = state->getMavis()->makeInst(opcode, state);
            state->setCurrentInst(inst);
            // Set next PC, can be overidden by a branch/jump instruction or an exception
            state->setNextPc(state->getPc() + opcode_size);
        }
        catch (const mavis::UnknownOpcode & e)
        {
            THROW_ILLEGAL_INSTRUCTION;
        }

        return nullptr;
    }

    void Fetch::advanceSim_()
    {
        // Run
        ActionGroup* next_action_group = &fetch_action_group_;
        while (next_action_group && (next_action_group->hasTag(ActionTags::STOP_SIM_TAG) == false))
        {
            DLOG(next_action_group);
            next_action_group = next_action_group->execute(state_);
        }
        // End of sim
    }
} // namespace atlas
