#include "sim/AtlasSim.hpp"
#include "core/AtlasState.hpp"
#include "include/AtlasTypes.hpp"
#include "core/PageTableEntry.hpp"
#include <bitset>
#include "sparta/utils/SpartaTester.hpp"

class AtlasTranslateTester
{
public:
    AtlasTranslateTester()
    {
        // Create the simulator
        const std::string workload = "";
        const uint64_t ilimit = 0;
        atlas_sim_.reset(new atlas::AtlasSim(&scheduler_,
                                             workload,
                                             ilimit
        ));

        atlas_sim_->buildTree();
        atlas_sim_->configureTree();
        atlas_sim_->finalizeTree();

        state_ = atlas_sim_->getAtlasState();
        translate_unit_ = state_->getTranslateUnit();
    }

    void testAtlasTranslationState()
    {
        const atlas::Addr va = 0x1000;
        const uint32_t access_size = 4;
        // TODO: Add test for AtlasTranslationState state machine
        
        atlas::AtlasTranslationState* translation_state = state_->getTranslationState();
        EXPECT_THROW(translation_state->getTranslationRequest());   //request should be invalid hence expedct throw;

        EXPECT_THROW(translate_unit_->translate_(state_));  //this will call setTransaltionRequest which should fail as transalationRequest is invalid

        //    - Make a second request without resolving the first request
        translation_state->makeTranslationRequest(va, access_size); //1st request
        EXPECT_THROW(translation_state->makeTranslationRequest(va, access_size);); //2nd request

        //    - Try to get a result before it is ready
        EXPECT_THROW(translation_state->getTranslationResult());    //getting the result before it is ready, hence expect failure

        //    - More?
        //
        // TODO: Add tests for AtlasTranslationState request/result validation
        //    - Make a misaligned request (is it supported?)
        //    - Make a request with an invalid size (too large, too small, not power of 2, etc.)
        //    - Set the result with the wrong size
        //    - More?
    }

    void testBaremetalTranslation()
    {
        std::cout << "Testing baremetal translation\n" << std::endl;

        // Make translation request
        atlas::AtlasTranslationState* translation_state = state_->getTranslationState();
        const atlas::Addr va = 0x1000;
        const uint32_t access_size = 4;
        std::cout << "Translation request:" << std::endl;
        std::cout << "    VA: 0x" << std::hex << va;
        std::cout << ", Access size: " << std::dec << access_size << std::endl;
        translation_state->makeTranslationRequest(va, access_size);

        // Execute translation
        auto next_action_group = translate_unit_->translate_(state_);
        EXPECT_EQUAL(next_action_group, nullptr);

        // Get translation result
        const atlas::AtlasTranslationState::TranslationResult result = translation_state->getTranslationResult();
        std::cout << "Translation result:" << std::endl;
        std::cout << "    PA: 0x" << std::hex << result.physical_addr;
        std::cout << ", Access size: " << std::dec << result.size << "\n\n";

        // Test translation result
        EXPECT_EQUAL(result.physical_addr, va);
        EXPECT_EQUAL(result.size, access_size);
    }

    void testPageTableEntryCreation()
    {
        const bool v = true;
        const bool r = true;
        const bool w = true;
        const bool x = true;
        const bool u = true;
        const bool g = true;
        const bool a = true;
        const bool d = true;
        uint8_t rsw = 2;
        uint16_t ppn0 = 123;
        uint16_t ppn1 = 123;
        uint32_t examplePTEValue;
        examplePTEValue = (ppn1 << 20) | (ppn0 << 10) | ((rsw & TWO_BIT_MASK) << 8) | (d << 7) | (a << 6) | (g << 5) | (u << 4) | (x << 3) | (w << 2) | (r << 1) | (v);
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32PageTableEntry(examplePTEValue);
        EXPECT_TRUE(sv32PageTableEntry.isValid());
        EXPECT_TRUE(sv32PageTableEntry.canRead());
        EXPECT_TRUE(sv32PageTableEntry.canWrite());
        EXPECT_TRUE(sv32PageTableEntry.canExecute());
        EXPECT_TRUE(sv32PageTableEntry.isUserMode());
        EXPECT_TRUE(sv32PageTableEntry.isGlobal());
        EXPECT_TRUE(sv32PageTableEntry.isAccessed());   
        EXPECT_TRUE(sv32PageTableEntry.isDirty());
        EXPECT_EQUAL(sv32PageTableEntry.getRSW(), rsw);
        EXPECT_EQUAL(sv32PageTableEntry.getPPN0(), ppn0);
        EXPECT_EQUAL(sv32PageTableEntry.getPPN1(), ppn1);
    }

    void testSv32Translation()
    {
        //const atlas::Addr pa = 0x1000;
        //const uint64_t value = 0xdeadbeef;
        //state_->writeMemory<uint64_t>(pa, value);
    }

private:
    sparta::Scheduler scheduler_;
    std::unique_ptr<atlas::AtlasSim> atlas_sim_;

    atlas::AtlasState* state_ = nullptr;
    atlas::Translate* translate_unit_ = nullptr;
};

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    AtlasTranslateTester translate_tester;
    translate_tester.testBaremetalTranslation();
    translate_tester.testPageTableEntryCreation();
    translate_tester.testAtlasTranslationState();

    REPORT_ERROR;
    return (int)ERROR_CODE;
}
