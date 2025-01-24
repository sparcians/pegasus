#include "sim/AtlasSim.hpp"

#include "core/AtlasState.hpp"
#include "core/translate/PageTable.hpp"

#include "include/AtlasTypes.hpp"
#include "include/CSRNums.hpp"

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
        atlas_sim_.reset(new atlas::AtlasSim(&scheduler_, workload, ilimit));

        atlas_sim_->buildTree();
        atlas_sim_->configureTree();
        atlas_sim_->finalizeTree();

        state_ = atlas_sim_->getAtlasState();
        translate_unit_ = state_->getTranslateUnit();
    }

    void testAtlasTranslationState()
    {
        std::cout << "Testing AtlasTranslationState class" << std::endl;
        std::cout << std::endl;
        const atlas::Addr vaddr = 0x1000;
        const uint32_t access_size = 4;
        // TODO: Add test for AtlasTranslationState state machine

        atlas::AtlasTranslationState* translation_state = state_->getTranslationState();

        // Request should be invalid and throw an exception
        EXPECT_THROW(translation_state->getTranslationRequest());

        // This will call setTransaltionRequest which should fail as transalationRequest is invalid
        EXPECT_THROW(translate_unit_->translate_<atlas::MMUMode::BAREMETAL>(state_));

        // Make a second request without resolving the first request
        translation_state->makeTranslationRequest(vaddr, access_size);
        EXPECT_THROW(translation_state->makeTranslationRequest(vaddr, access_size););

        // Try to get a result before it is ready will fail
        EXPECT_THROW(translation_state->getTranslationResult());

        // Clear current request so subsequent tests will pass
        translation_state->setTranslationResult(vaddr, access_size);

        // TODO: Add tests for AtlasTranslationState request/result validation
        //    - Make a misaligned request (is it supported?)
        //    - Make a request with an invalid size (too large, too small, not power of 2, etc.)
        //    - Set the result with the wrong size
        //    - More?
    }

    void testPageTableEntry() // unit test for PageTableEntry.cpp
    {
        std::cout << "Testing PageTableEntry class" << std::endl;

        // sv32
        {
            const bool v = true;
            const bool r = true;
            const bool w = true;
            const bool x = true;
            const bool u = true;
            const bool g = true;
            const bool a = true;
            const bool d = true;
            const uint32_t rsw = 2;
            const uint32_t ppn0 = 123;
            const uint32_t ppn1 = 456;
            const uint32_t pte_val =
                (ppn1 << atlas::Sv32Pte::ppn1::low_bit) | (ppn0 << atlas::Sv32Pte::ppn0::low_bit)
                | (rsw << atlas::Sv32Pte::rsw::low_bit) | (d << atlas::Sv32Pte::dirty::low_bit)
                | (a << atlas::Sv32Pte::accessed::low_bit) | (g << atlas::Sv32Pte::global::low_bit)
                | (u << atlas::Sv32Pte::user::low_bit) | (x << atlas::Sv32Pte::execute::low_bit)
                | (w << atlas::Sv32Pte::write::low_bit) | (r << atlas::Sv32Pte::read::low_bit)
                | (v);

            std::cout << "Creating sv32 PTE: " << HEX8(pte_val) << std::endl;
            atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte(pte_val);

            EXPECT_TRUE(sv32_pte.isValid());
            EXPECT_TRUE(sv32_pte.canRead());
            EXPECT_TRUE(sv32_pte.canWrite());
            EXPECT_TRUE(sv32_pte.canExecute());
            EXPECT_TRUE(sv32_pte.isUserMode());
            EXPECT_TRUE(sv32_pte.isGlobal());
            EXPECT_TRUE(sv32_pte.isAccessed());
            EXPECT_TRUE(sv32_pte.isDirty());
            EXPECT_EQUAL(sv32_pte.getRsw(), rsw);
            EXPECT_EQUAL(sv32_pte.getPpnField(0), ppn0);
            EXPECT_EQUAL(sv32_pte.getPpnField(1), ppn1);
        }
        // sv39
        {
            const bool v = true;
            const bool r = true;
            const bool w = false;
            const bool x = false;
            const bool u = true;
            const bool g = true;
            const bool a = true;
            const bool d = true;
            const uint32_t rsw = 2;
            const uint64_t ppn0 = 123;
            const uint64_t ppn1 = 456;
            const uint64_t ppn2 = 789;
            const uint64_t pte_val =
                (ppn2 << atlas::Sv39Pte::ppn2::low_bit) | (ppn1 << atlas::Sv39Pte::ppn1::low_bit)
                | (ppn0 << atlas::Sv39Pte::ppn0::low_bit) | (rsw << atlas::Sv39Pte::rsw::low_bit)
                | (d << atlas::Sv39Pte::dirty::low_bit) | (a << atlas::Sv39Pte::accessed::low_bit)
                | (g << atlas::Sv39Pte::global::low_bit) | (u << atlas::Sv39Pte::user::low_bit)
                | (x << atlas::Sv39Pte::execute::low_bit) | (w << atlas::Sv39Pte::write::low_bit)
                | (r << atlas::Sv39Pte::read::low_bit) | (v);

            std::cout << "Creating sv64 PTE: " << HEX16(pte_val) << std::endl;
            atlas::PageTableEntry<atlas::RV64, atlas::MMUMode::SV39> sv39_pte(pte_val);

            EXPECT_TRUE(sv39_pte.isValid());
            EXPECT_TRUE(sv39_pte.canRead());
            EXPECT_FALSE(sv39_pte.canWrite());
            EXPECT_FALSE(sv39_pte.canExecute());
            EXPECT_TRUE(sv39_pte.isUserMode());
            EXPECT_TRUE(sv39_pte.isGlobal());
            EXPECT_TRUE(sv39_pte.isAccessed());
            EXPECT_TRUE(sv39_pte.isDirty());
            EXPECT_EQUAL(sv39_pte.getRsw(), rsw);
            EXPECT_EQUAL(sv39_pte.getPpnField(0), ppn0);
            EXPECT_EQUAL(sv39_pte.getPpnField(1), ppn1);
            EXPECT_EQUAL(sv39_pte.getPpnField(2), ppn2);
        }
    }

    void testPageTable() // unit test for PageTable.cpp
    {
        std::cout << "\nTesting PageTable class" << std::endl;
        constexpr uint32_t PTE_SIZE = sizeof(atlas::RV32);
        const uint32_t base_addr = 0xFFFF0000;
        atlas::PageTable<atlas::RV32, atlas::MMUMode::SV32> pt(base_addr);

        // Valid, Readable, Writable, and Executable (0000 1010 1011 1100 0001 0010 1111 0000 1111)
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte1(0x7B1EEFF);
        // Valid, Readable, Writable, and Executable (0000 1010 1011 1100 0001 0010 1111 0000 1111)
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte2(0xABC12FF);
        // Valid, Read-only (0111 1111 0000 0011 1101 0100 1100 0011)
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte3(0x7F03D4C3);
        // Valid,
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> sv32_pte4(0xABC12FF);

        std::cout << "Creating page table with 4 PTEs:" << std::endl;
        std::cout << "    PTE1: " << sv32_pte1 << std::endl;
        std::cout << "    PTE2: " << sv32_pte2 << std::endl;
        std::cout << "    PTE3: " << sv32_pte3 << std::endl;
        std::cout << "    PTE4: " << sv32_pte4 << std::endl;
        std::cout << std::endl;

        pt.addEntry(base_addr + 1 * PTE_SIZE, sv32_pte1);
        pt.addEntry(base_addr + 10 * PTE_SIZE, sv32_pte2);
        pt.addEntry(base_addr + 100 * PTE_SIZE, sv32_pte3);
        pt.addEntry(base_addr + 1023 * PTE_SIZE, sv32_pte4);

        EXPECT_EQUAL(pt.getEntry(base_addr + 1 * PTE_SIZE).getPpn(), sv32_pte1.getPpn());
        EXPECT_EQUAL(pt.getEntry(base_addr + 10 * PTE_SIZE).getPpn(), sv32_pte2.getPpn());
        EXPECT_EQUAL(pt.getEntry(base_addr + 100 * PTE_SIZE).getPpn(), sv32_pte3.getPpn());
        EXPECT_EQUAL(pt.getEntry(base_addr + 1023 * PTE_SIZE).getPpn(), sv32_pte4.getPpn());

        // Page table has reached its maximum capacity/PageTable index out of bound!
        EXPECT_THROW(pt.addEntry(base_addr + 1024 * PTE_SIZE, sv32_pte4));

        // Entry is not present at the provided index
        EXPECT_THROW(pt.getEntry(base_addr + 1022 * PTE_SIZE).getPpn());

        // Invalid index
        EXPECT_THROW(pt.removeEntry(base_addr + 1044 * PTE_SIZE));

        EXPECT_TRUE(pt.contains(base_addr + 1023 * PTE_SIZE));
        pt.removeEntry(base_addr + 1023 * PTE_SIZE);
        EXPECT_FALSE(pt.contains(base_addr + 1023 * PTE_SIZE));

        EXPECT_TRUE(pt.contains(base_addr + 100 * PTE_SIZE));
        pt.removeEntry(base_addr + 100 * PTE_SIZE);
        EXPECT_FALSE(pt.contains(base_addr + 100 * PTE_SIZE));

        EXPECT_TRUE(pt.contains(base_addr + 10 * PTE_SIZE));
        pt.removeEntry(base_addr + 10 * PTE_SIZE);
        EXPECT_FALSE(pt.contains(base_addr + 10 * PTE_SIZE));

        EXPECT_TRUE(pt.contains(base_addr + 1 * PTE_SIZE));
        pt.removeEntry(base_addr + 1 * PTE_SIZE);
        EXPECT_FALSE(pt.contains(base_addr + 1 * PTE_SIZE));
    }

    void testBaremetalTranslation()
    {
        std::cout << "Testing baremetal translation\n" << std::endl;

        // Make translation request
        atlas::AtlasTranslationState* translation_state = state_->getTranslationState();
        const atlas::Addr vaddr = 0x1000;
        const uint32_t access_size = 4;
        std::cout << "Translation request:" << std::endl;
        std::cout << "    VA: 0x" << std::hex << vaddr;
        std::cout << ", Access size: " << std::dec << access_size << std::endl;
        translation_state->makeTranslationRequest(vaddr, access_size);

        // Execute translation
        auto next_action_group =
            translate_unit_->translate_<atlas::RV64, atlas::MMUMode::BAREMETAL>(state_);
        EXPECT_EQUAL(next_action_group, nullptr);

        // Get translation result
        const atlas::AtlasTranslationState::TranslationResult result =
            translation_state->getTranslationResult();
        std::cout << "Translation result:" << std::endl;
        std::cout << "    PA: 0x" << std::hex << result.physical_addr;
        std::cout << ", Access size: " << std::dec << result.size << "\n\n";

        // Test translation result
        EXPECT_EQUAL(result.physical_addr, vaddr);
        EXPECT_EQUAL(result.size, access_size);
    }

    void testSv32Translation()
    {
        std::cout << "Testing sv32 translation\n" << std::endl;
        const uint32_t PTE_SIZE = sizeof(atlas::RV32);

        // Virtual address to test
        // {vpn[1]-->0x50-->d(80) , vpn[1]-->0xFF-->d(1023), offset-->0xABC-->d(2748)}
        uint32_t vaddr = 0x143FFABC;

        // Physical address of root page table entry
        uint32_t satpBaseAddress = 0xFFFF0000;
        uint32_t pdeAddress = satpBaseAddress + (80 * PTE_SIZE);
        uint32_t pdeVal = 0x7B1EEFF;
        uint32_t pageTableBaseAddr = (pdeVal >> 10) << 10; // 7B1EC00
        uint32_t pteAddress = pageTableBaseAddr + (1023 * PTE_SIZE);
        uint32_t pteVal = 0x7F03D4C3;
        const atlas::Addr phyMemoryBaseAddr = (pteVal >> 10) << 10; // 0x7F03D400

        // Expected results of translation
        const atlas::Addr expected_paddr = (phyMemoryBaseAddr + (2748 * PTE_SIZE));
        const uint64_t expected_val = 0xABCD1234;
        state_->writeMemory<uint64_t>(expected_paddr, expected_val);

        atlas::PageTable<atlas::RV32, atlas::MMUMode::SV32> pageDirectory(satpBaseAddress);
        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> pageDirectoryEntry(pdeVal);
        pageDirectory.addEntry(pdeAddress, pageDirectoryEntry);

        atlas::PageTable<atlas::RV32, atlas::MMUMode::SV32> pageTable(pageTableBaseAddr);

        atlas::PageTableEntry<atlas::RV32, atlas::MMUMode::SV32> PageTableEntry(pteVal);
        pageTable.addEntry(pteAddress, PageTableEntry);

        // Write PTEs to memory
        state_->writeMemory<uint64_t>(pdeAddress, pdeVal);
        state_->writeMemory<uint64_t>(pteAddress, pteVal);

        // Set SATP value
        state_->getCsrRegister(atlas::CSR::SATP::reg_num)->dmiWrite<uint64_t>(satpBaseAddress);

        // Make translation request
        atlas::AtlasTranslationState* translation_state = state_->getTranslationState();
        const size_t access_size = 4;
        translation_state->makeTranslationRequest(vaddr, access_size);
        std::cout << "Translation request:" << std::endl;
        std::cout << "    VA: 0x" << std::hex << vaddr;
        std::cout << ", Access size: " << std::dec << access_size << std::endl;

        // Translate!
        translate_unit_->translate_<atlas::RV32, atlas::MMUMode::SV32>(state_);

        // Get translation result
        const atlas::AtlasTranslationState::TranslationResult result =
            translation_state->getTranslationResult();
        std::cout << "Translation result:" << std::endl;
        std::cout << "    PA: 0x" << std::hex << result.physical_addr;
        std::cout << ", Access size: " << std::dec << result.size << "\n\n";

        // Test translation result
        EXPECT_EQUAL(result.physical_addr, expected_paddr);
        EXPECT_EQUAL(state_->readMemory<uint64_t>(result.physical_addr), expected_val);
    }

  private:
    sparta::Scheduler scheduler_;
    std::unique_ptr<atlas::AtlasSim> atlas_sim_;

    atlas::AtlasState* state_ = nullptr;
    atlas::Translate* translate_unit_ = nullptr;
};

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    AtlasTranslateTester translate_tester;

    translate_tester.testAtlasTranslationState();
    translate_tester.testPageTableEntry();
    translate_tester.testPageTable();

    translate_tester.testBaremetalTranslation();
    translate_tester.testSv32Translation();

    REPORT_ERROR;
    return (int)ERROR_CODE;
}
