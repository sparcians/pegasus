#include "TestComponents.hpp"
#include "include/ActionTags.hpp"

#include "sparta/utils/SpartaTester.hpp"

void runSim(atlas::AtlasState* state, atlas::ActionGroup* atlas_core,
            const uint32_t expected_num_insts, const uint32_t expected_num_actions)
{
    atlas::ActionGroup* action_group = atlas_core;
    while (action_group)
    {
        action_group = action_group->execute(state);
    }

    EXPECT_EQUAL(expected_num_insts, state->getNumInstsExecuted());
    EXPECT_EQUAL(expected_num_actions, state->getNumActionsExecuted());

    // Reset for next test
    state->reset();
}

int main()
{
    atlas::AtlasState state;

    // Create core ActionGroup
    atlas::ActionGroup fetch{"fetch"};
    atlas::ActionGroup decode{"decode"};
    atlas::ActionGroup execute{"execute"};

    FetchUnit fetch_unit;
    DecodeUnit decode_unit;
    ExecuteUnit execute_unit{&fetch};

    atlas::Action fetch_action = atlas::Action::createAction<&FetchUnit::fetch_inst>(
        &fetch_unit, fetch_unit.getName().c_str());
    fetch_action.addTag(atlas::ActionTags::FETCH_TAG);
    atlas::Action decode_action = atlas::Action::createAction<&DecodeUnit::decode_inst>(
        &decode_unit, decode_unit.getName().c_str());
    decode_action.addTag(atlas::ActionTags::DECODE_TAG);
    atlas::Action execute_action = atlas::Action::createAction<&ExecuteUnit::execute_inst>(
        &execute_unit, execute_unit.getName().c_str());
    execute_action.addTag(atlas::ActionTags::EXECUTE_TAG);

    fetch.addAction(fetch_action);
    decode.addAction(decode_action);
    execute.addAction(execute_action);

    // Connect the ActionGroups together
    fetch.setNextActionGroup(&decode);
    decode.setNextActionGroup(&execute);
    execute.setNextActionGroup(&fetch);

    // Dummy Action
    const std::string dummy_action_name = "Dummy";
    atlas::Action dummy_action = atlas::Action::createAction<&atlas::AtlasState::dummy_action>(
        &state, dummy_action_name.c_str());
    const atlas::ActionTagType DUMMY_TAG = atlas::ActionTagFactory::createTag("DUMMY_TAG");
    dummy_action.addTag(DUMMY_TAG);

    //
    // Test the basics, run until wfi
    //
    std::cout << "TEST: Basics\n";
    std::cout << fetch << std::endl;
    std::cout << decode << std::endl;
    std::cout << execute << std::endl;
    std::cout << std::endl;
    runSim(&state, &fetch, 6, 24);

    //
    // Add a dummy Action after Fetch
    //
    fetch.insertActionAfter(dummy_action, atlas::ActionTags::FETCH_TAG);
    std::cout << "TEST: Insert Action After\n";
    std::cout << fetch << std::endl;
    std::cout << std::endl;
    runSim(&state, &fetch, 6, 30);

    //
    // Add another dummy Action before Execute
    //
    execute.insertActionBefore(dummy_action, atlas::ActionTags::EXECUTE_TAG);
    std::cout << "TEST: Insert Action After\n";
    std::cout << execute << std::endl;
    std::cout << std::endl;
    runSim(&state, &fetch, 6, 36);

    //
    // Remove the dummy Actions that were added
    //
    std::cout << "TEST: Remove Actions\n";
    fetch.removeAction(DUMMY_TAG);
    execute.removeAction(DUMMY_TAG);
    std::cout << fetch << std::endl;
    std::cout << decode << std::endl;
    std::cout << execute << std::endl;
    runSim(&state, &fetch, 6, 24);

    //
    // Trying to remove an Action that is not in the ActionGroup will fail
    //
    std::cout << "TEST: Remove Action Fails\n";
    EXPECT_THROW(decode.removeAction(DUMMY_TAG));

    //
    // Bypass the Decode ActionGroup
    //
    //
    fetch.setNextActionGroup(&execute);
    std::cout << "TEST: Bypass ActionGroup\n";
    std::cout << std::endl;
    runSim(&state, &fetch, 6, 18);
    fetch.setNextActionGroup(&decode);

    //
    // Create another ActionGroup for Translation
    //
    atlas::ActionGroup translate{"translate"};
    TranslateUnit translate_unit;
    atlas::Action translate_action = atlas::Action::createAction<&TranslateUnit::translate_addr>(
        &translate_unit, translate_unit.getName().c_str());
    translate_action.addTag(atlas::ActionTags::INST_TRANSLATE_TAG);
    translate.addAction(translate_action);
    fetch.setNextActionGroup(&translate);
    translate.setNextActionGroup(&decode);
    std::cout << fetch << std::endl;
    std::cout << translate << std::endl;
    std::cout << decode << std::endl;
    std::cout << execute << std::endl;
    std::cout << std::endl;
    runSim(&state, &fetch, 6, 30);

    REPORT_ERROR;
    return ERROR_CODE;
}
