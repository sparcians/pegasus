
#include "sim/PegasusSim.hpp"
#include "core/PegasusState.hpp"
#include "include/PegasusTypes.hpp"
#include "core/translate/ExecutionPage.hpp"

#include <cinttypes>

int main()
{
    sparta::Scheduler scheduler;

    pegasus::PegasusSim pegasus_sim(&scheduler);

    sparta::app::SimulationConfiguration config;
    config.processParameter("top.*.*.params.quantum", "20000000");
    pegasus_sim.configure(0, nullptr, &config);
    pegasus_sim.buildTree();
    pegasus_sim.configureTree();
    pegasus_sim.finalizeTree();

    // pegasus_sim.installTaps({sparta::log::TapDescriptor("top", "info", "1")});
    // pegasus_sim.installTaps({sparta::log::TapDescriptor("top", "debug", "1")});

    auto state = pegasus_sim.getPegasusCore(0)->getPegasusState();
    auto fetch_unit = state->getFetchUnit();
    auto fetch_action_group = fetch_unit->getFetchActionGroup();

    auto execute_unit = state->getExecuteUnit();
    auto execute_action_group = execute_unit->getActionGroup();


    ////////////////////////////////////////////////////////////////////////////////
    // Simple test -- for loop counter: one add + branch to add then stop

    // Create a simple translation page
    pegasus::Addr virt_page_address = 0x00000000C0000000;
    pegasus::Addr phys_page_address = 0x0000000080000000;
    state->setPc(virt_page_address);

    // Write this program:
    // 0x0000000080000000
    //     009890b7          	lui	ra,0x989
    //     6800809b          	addiw	ra,ra,1664 # 989680 <main-0x7f676980>
    //     4105              	li	sp,1
    state->writeMemory(phys_page_address,       0x009890b7u);
    state->writeMemory(phys_page_address + 0x4, 0x6800809bu);
    state->writeMemory(phys_page_address + 0x8, 0x4105u);

    // 000000008000000a <loop>:
    //     0105               	addi	sp,sp,1
    //     fe111fe3          	bne	sp,ra,8000000a <loop>
    state->writeMemory(phys_page_address + 0xa, 0x0105u);
    state->writeMemory(phys_page_address + 0xc, 0xfe111fe3u);

    auto * tstate = state->getFetchTranslationState();
    pegasus::ActionGroup* next_action_group = fetch_unit->getFetchActionGroup();
    uint32_t break_out = 10000000;
    tstate->makeRequest(virt_page_address, 4);
    tstate->setResult(virt_page_address, phys_page_address, 4, pegasus::translate_types::PageSize::SIZE_4K);

    pegasus::ExecutionPage translation_page(tstate->getResult(),
                                             fetch_action_group,
                                             execute_action_group);

    fetch_action_group->setNextActionGroup(translation_page.getExecutionPageActionGroup());
    do {
        next_action_group = next_action_group->execute(state);
        if (state->getSimState()->inst_count == break_out) {
            std::cout << std::dec << "Hit " << break_out
                      << " cnt: " << state->getSimState()->inst_count
                      << " addr: " << std::hex << state->getPc()
                      << " sp: "   << std::dec << state->getIntRegister(2)->read<uint64_t>()
                      << std::endl;
            break_out += 10000000;
        }
    } while (nullptr != next_action_group);
    //} while (state->getPc() != 0x00000000c0000010ull);

    std::cout << "Executed: " << state->getSimState()->inst_count << std::endl;

    ////////////////////////////////////////////////////////////////////////////////
    // Cross a 4M page with a 32-bit opcode that crosses a 4k page
    // VA -> PA mapping:
    //    0xFFFF_FFFF_FFFF_0040_0000 -> 0x0000_0000_0000_0840_0000

    virt_page_address = 0xFFFFFFFFF0000000ull | pegasus::translate_types::getPageMask(pegasus::translate_types::PageSize::SIZE_4M);
    phys_page_address = 0x0000000008000000ull | pegasus::translate_types::getPageMask(pegasus::translate_types::PageSize::SIZE_4M);

    tstate->makeRequest(virt_page_address, 4);
    tstate->setResult(virt_page_address, phys_page_address, 4, pegasus::translate_types::PageSize::SIZE_4M);

    pegasus::ExecutionPage translation_page_split(tstate->getResult(),
                                                   fetch_action_group,
                                                   execute_action_group);

    fetch_action_group->setNextActionGroup(translation_page_split.getExecutionPageActionGroup());

    // Write the same program but shifted to cross a page:
    // 0x0000000000008400ff2:
    //     009890b7          	lui	ra,0x989
    // 0x0000000000008400ff6:
    //     6800809b          	addiw	ra,ra,1664 # 989680 <main-0x7f676980>
    // 0x0000000000008400ffa:
    //     4105              	c.li	sp,1
    //     0105               	c.addi	sp,sp,1
    // 0x0000000000008400ffe <loop>:
    //     fe111fe3          	bne	sp,ra,8000000a <loop>
    const auto page_edge = 0xff2;
    state->writeMemory(phys_page_address + page_edge,       0x009890b7u); // 0xf2 -> 0xf6
    state->writeMemory(phys_page_address + page_edge + 0x4, 0x6800809bu); // 0xf6 -> 0xfa
    state->writeMemory(phys_page_address + page_edge + 0x8, uint16_t(0x4105)); // 0xfa -> 0xfe, c.li
    state->writeMemory(phys_page_address + page_edge + 0xa, uint16_t(0x0105)); // 0xfa -> 0xfe, c.addi

    state->setPc(virt_page_address + page_edge);

    // Have the branch cross the page
    // 0x0000000000008400ffe:
    //     fe111fe3          	bne	sp,ra,8000000a <loop>
    state->writeMemory(phys_page_address + page_edge + 0xc, uint16_t(0xfe11)); // 0xfe -> 0x00, 1st half
    state->writeMemory(phys_page_address + page_edge + 0xe, uint16_t(0x1fe3)); // 0x00 -> 0x02, 2nd half

    pegasus_sim.installTaps({sparta::log::TapDescriptor("top", "info", "1")});
    pegasus_sim.installTaps({sparta::log::TapDescriptor("top", "debug", "1")});

    next_action_group = fetch_unit->getFetchActionGroup();
    break_out = 30000000;
    do {
        next_action_group = next_action_group->execute(state);
        if (state->getSimState()->inst_count == break_out) {
            std::cout << std::dec << "Hit " << break_out
                      << " cnt: " << state->getSimState()->inst_count
                      << " addr: " << std::hex << state->getPc()
                      << " sp: "   << std::dec << state->getIntRegister(2)->read<uint64_t>()
                      << std::endl;
            break_out += 10000000;
        }
    } while (nullptr != next_action_group);

    std::cout << "Executed: " << state->getSimState()->inst_count << std::endl;

    return 0;
}
