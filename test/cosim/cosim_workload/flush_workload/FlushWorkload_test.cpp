#include "cosim/PegasusCoSim.hpp"
#include "cosim/CoSimEventPipeline.hpp"
#include "simdb/apps/AppManager.hpp"
#include "sparta/kernel/SleeperThread.hpp"
#include "sparta/utils/SpartaTester.hpp"

/// In this test, we will be running the same workload through two PegasusCoSim
/// instances. One will ONLY step forward and serve as the "truth" against which
/// we will validate registers, PC, and other state. The other instance will step
/// forward N times, then flush to the N-1 instruction. Then the comparison takes
/// place, and we continue until the end of the workload.

using namespace pegasus;
using namespace pegasus::cosim;

bool AdvanceSimTruth(PegasusCoSim& sim, CoreId core_id, HartId hart_id)
{
    auto state = sim.getPegasusCore(core_id)->getPegasusState(hart_id);

    // Return false if we cannot step forward (end of sim)
    if (state->getSimState()->sim_stopped)
    {
        return false;
    }

    auto start_pc = state->getPc();
    auto event = sim.step(core_id, hart_id);
    auto event_pc = event->getPc();
    auto event_next_pc = event->getNextPc();

    (void)start_pc;
    (void)event_pc;
    (void)event_next_pc;

    sim.commit(event);
    return true;
}

bool AdvanceSimTest(PegasusCoSim& sim, CoreId core_id, HartId hart_id)
{
    auto state = sim.getPegasusCore(core_id)->getPegasusState(hart_id);

    // Return false if we cannot step forward (end of sim)
    if (state->getSimState()->sim_stopped)
    {
        return false;
    }

    // Step N times, flush N-1 events, and commit the 1st step.
    const size_t max_steps_before_flush = 3;
    std::vector<EventAccessor> stepped_events;
    while (stepped_events.size() < max_steps_before_flush)
    {
        auto start_pc = state->getPc();
        auto event = sim.step(core_id, hart_id);
        auto event_pc = event->getPc();
        auto event_next_pc = event->getNextPc();

        (void)start_pc;
        (void)event_pc;
        (void)event_next_pc;

        stepped_events.push_back(event);
        if (event->isLastEvent())
        {
            break;
        }
    }

    while (stepped_events.size() > 1)
    {
        auto event = stepped_events.back();
        stepped_events.pop_back();

        constexpr bool flush_younger_only = false;
        sim.flush(event, flush_younger_only);
    }

    sim.commit(stepped_events.front());
    if (stepped_events.front()->isLastEvent())
    {
        return false;
    }

    return true;
}

template <typename XLEN>
bool Compare(PegasusCoSim& sim_truth, PegasusCoSim& sim_test,
             CoreId core_id, HartId hart_id)
{
    auto state_truth = sim_truth.getPegasusCore(core_id)->getPegasusState(hart_id);
    auto state_test = sim_test.getPegasusCore(core_id)->getPegasusState(hart_id);

    // Compare PegasusState member variables
    EXPECT_EQUAL(state_truth->getXlen(), state_test->getXlen());
    EXPECT_EQUAL(state_truth->isCompressionEnabled(), state_test->isCompressionEnabled());
    EXPECT_EQUAL(state_truth->getStopSimOnWfi(), state_test->getStopSimOnWfi());
    EXPECT_EQUAL(state_truth->getPc(), state_test->getPc());
    EXPECT_EQUAL(state_truth->getPcAlignment(), state_test->getPcAlignment());
    EXPECT_EQUAL(state_truth->getPrivMode(), state_test->getPrivMode());
    EXPECT_EQUAL(state_truth->getLdstPrivMode(), state_test->getLdstPrivMode());
    EXPECT_EQUAL(state_truth->getVirtualMode(), state_test->getVirtualMode());
    EXPECT_EQUAL(state_truth->hasHypervisor(), state_test->hasHypervisor());
    // TODO cnyce: pc alignment (and mask)

    // Compare sim state
    auto sim_state_truth = state_truth->getSimState();
    auto sim_state_test = state_test->getSimState();
    EXPECT_EQUAL(sim_state_truth->current_opcode, sim_state_test->current_opcode);
    EXPECT_EQUAL(sim_state_truth->current_uid, sim_state_test->current_uid);
    EXPECT_EQUAL(sim_state_truth->inst_count, sim_state_test->inst_count);
    EXPECT_EQUAL(sim_state_truth->sim_stopped, sim_state_test->sim_stopped);
    EXPECT_EQUAL(sim_state_truth->test_passed, sim_state_test->test_passed);
    EXPECT_EQUAL(sim_state_truth->workload_exit_code, sim_state_test->workload_exit_code);

    // Compare current inst
    auto inst_truth = state_truth->getCurrentInst();
    auto inst_test = state_test->getCurrentInst();

    auto has_inst_truth = (inst_truth != nullptr);
    auto has_inst_test = (inst_test != nullptr);
    EXPECT_EQUAL(has_inst_truth, has_inst_test);

    if (has_inst_truth && has_inst_test)
    {
        EXPECT_EQUAL(inst_truth->getUid(), inst_test->getUid());
        EXPECT_EQUAL(inst_truth->getOpcode(), inst_test->getOpcode());
        EXPECT_EQUAL(inst_truth->getOpcodeSize(), inst_test->getOpcodeSize());
        EXPECT_EQUAL(inst_truth->isMemoryInst(), inst_test->isMemoryInst());
        EXPECT_EQUAL(inst_truth->isChangeOfFlowInst(), inst_test->isChangeOfFlowInst());
        EXPECT_EQUAL(inst_truth->hasCsr(), inst_test->hasCsr());
        EXPECT_EQUAL(inst_truth->writesCsr(), inst_test->writesCsr());
        EXPECT_EQUAL(inst_truth->isStoreType(), inst_test->isStoreType());
        EXPECT_EQUAL(inst_truth->getMnemonic(), inst_test->getMnemonic());
        EXPECT_EQUAL(inst_truth->dasmString(), inst_test->dasmString());
    }

    // Compare registers
    auto compare_simple_regs = [](RegisterSet* rset_truth,
                                  RegisterSet* rset_test)
    {
        EXPECT_EQUAL(rset_truth->getNumRegisters(), rset_test->getNumRegisters());
        for (uint32_t reg_id = 0; reg_id < rset_truth->getNumRegisters(); ++reg_id)
        {
            auto reg_value_truth = rset_truth->readRegister<XLEN>(reg_id);
            auto reg_value_test = rset_test->readRegister<XLEN>(reg_id);
            EXPECT_EQUAL(reg_value_truth, reg_value_test);
        }
    };

    compare_simple_regs(state_truth->getIntRegisterSet(), state_test->getIntRegisterSet());
    compare_simple_regs(state_truth->getFpRegisterSet(), state_test->getFpRegisterSet());
    compare_simple_regs(state_truth->getVecRegisterSet(), state_test->getVecRegisterSet());

    auto compare_csr_regs = [](RegisterSet* rset_truth,
                               RegisterSet* rset_test)
    {
        EXPECT_EQUAL(rset_truth->getNumRegisters(), rset_test->getNumRegisters());
        for (uint32_t reg_id = 0; reg_id < rset_truth->getNumRegisters(); ++reg_id)
        {
            auto reg_truth = rset_truth->getRegister(reg_id);
            auto reg_test = rset_test->getRegister(reg_id);
            EXPECT_TRUE((reg_truth && reg_test) || (!reg_truth && !reg_test));

            if (reg_truth && reg_test)
            {
                auto reg_value_truth = reg_truth->dmiRead<XLEN>();
                auto reg_value_test = reg_test->dmiRead<XLEN>();
                EXPECT_EQUAL(reg_value_truth, reg_value_test);
            }
        }
    };

    compare_csr_regs(state_truth->getCsrRegisterSet(), state_test->getCsrRegisterSet());

    // Compare vector config
    auto vec_config_truth = state_truth->getVectorConfig();
    auto vec_config_test = state_test->getVectorConfig();
    EXPECT_EQUAL(vec_config_truth->getVLEN(), vec_config_test->getVLEN());
    EXPECT_EQUAL(vec_config_truth->getLMUL(), vec_config_test->getLMUL());
    EXPECT_EQUAL(vec_config_truth->getSEW(), vec_config_test->getSEW());
    EXPECT_EQUAL(vec_config_truth->getVTA(), vec_config_test->getVTA());
    EXPECT_EQUAL(vec_config_truth->getVMA(), vec_config_test->getVMA());
    EXPECT_EQUAL(vec_config_truth->getVL(), vec_config_test->getVL());
    EXPECT_EQUAL(vec_config_truth->getVSTART(), vec_config_test->getVSTART());
    EXPECT_EQUAL(vec_config_truth->getVLMAX(), vec_config_test->getVLMAX());

    // Compare memory
    auto pipeline_truth = sim_truth.getEventPipeline(core_id, hart_id);
    auto pipeline_test = sim_test.getEventPipeline(core_id, hart_id);

    auto last_event_truth = pipeline_truth->getLastEvent();
    auto last_event_test = pipeline_test->getLastEvent();

    const auto& mem_reads_truth = last_event_truth->getMemoryReads();
    const auto& mem_reads_test = last_event_test->getMemoryReads();
    EXPECT_EQUAL(mem_reads_truth, mem_reads_test);

    const auto& mem_writes_truth = last_event_truth->getMemoryWrites();
    const auto& mem_writes_test = last_event_test->getMemoryWrites();
    EXPECT_EQUAL(mem_writes_truth, mem_writes_test);

    auto mem_writes_n = std::min(mem_writes_truth.size(), mem_writes_test.size());
    for (size_t i = 0; i < mem_writes_n; ++i)
    {
        const auto& write_truth = mem_writes_truth[i];
        const auto paddr = write_truth.paddr;
        auto mem_value_truth = state_truth->readMemory<XLEN>(paddr);
        auto mem_value_test = state_test->readMemory<XLEN>(paddr);
        EXPECT_EQUAL(mem_value_truth, mem_value_test);
    }

    // Compare enabled extensions
    auto & extensions_map_truth = state_truth->getExtensionManager().getEnabledExtensions();
    auto & extensions_map_test = state_test->getExtensionManager().getEnabledExtensions();

    std::set<std::string> ext_names;
    for (auto it = extensions_map_truth.begin(); it != extensions_map_truth.end(); ++it)
    {
        ext_names.insert(it->first);
    }
    for (auto it = extensions_map_test.begin(); it != extensions_map_test.end(); ++it)
    {
        ext_names.insert(it->first);
    }

    for (const auto & ext_name : ext_names)
    {
        auto ext_enabled_truth = extensions_map_truth.isEnabled(ext_name);
        auto ext_enabled_test = extensions_map_test.isEnabled(ext_name);
        EXPECT_EQUAL(ext_enabled_truth, ext_enabled_test);
    }

    return ERROR_CODE == 0;
}

template <typename XLEN>
bool AdvanceAndCompare(PegasusCoSim& sim_truth, PegasusCoSim& sim_test,
                       CoreId core_id, HartId hart_id)
{
    // TODO cnyce
    //return AdvanceSimTruth(sim_truth, core_id, hart_id);

    auto stepped_truth = AdvanceSimTruth(sim_truth, core_id, hart_id);
    auto stepped_test = AdvanceSimTest(sim_test, core_id, hart_id);

    EXPECT_EQUAL(stepped_truth, stepped_test);
    if (stepped_truth && stepped_test)
    {
        return Compare<XLEN>(sim_truth, sim_test, core_id, hart_id);
    }
    return false;
}

int main(int argc, char** argv)
{
    const std::string workload = argc >= 2 ? argv[1] : "";
    if (workload.empty())
    {
        std::cout << "Must supply workload!" << std::endl;
        return 1;
    }

    std::string arch;
    if (workload.find("rv32") == 0)
    {
        arch = "rv32";
    }
    else if (workload.find("rv64") == 0)
    {
        arch = "rv64";
    }
    else
    {
        std::cout << "Workload must start with 'rv32' or 'rv64' to indicate architecture!"
                  << std::endl;
        return 1;
    }

    // Disable sleeper thread so we can run two simulations at once.
    sparta::SleeperThread::disableForever();

    const uint64_t ilimit = 0;

    sparta::Scheduler scheduler_truth;
    PegasusCoSim cosim_truth(&scheduler_truth, ilimit, workload, "cosim_truth.db");

    sparta::Scheduler scheduler_test;
    PegasusCoSim cosim_test(&scheduler_test, ilimit, workload, "cosim_test.db");

    const pegasus::CoreId core_id = 0;
    const pegasus::HartId hart_id = 0;
    std::string exception_str;

    auto advance_and_compare = (arch == "rv32") ?
        AdvanceAndCompare<uint32_t> :
        AdvanceAndCompare<uint64_t>;

    size_t step_count = 0;
    try
    {
        while (true)
        {
            ++step_count;
            if (!advance_and_compare(cosim_truth, cosim_test, core_id, hart_id))
            {
                break;
            }
        }
        std::cout << "Completed " << step_count << " steps." << std::endl;
    }
    catch (const std::exception & ex)
    {
        exception_str = ex.what();
        std::cout << "Exception caught on step " << step_count << ": " << exception_str << std::endl;
    }

    // Shutdown pipelines
    cosim_truth.finish();
    cosim_test.finish();
    EXPECT_EQUAL(cosim_truth.getEventPipeline(core_id, hart_id)->getNumCached(), 0);
    EXPECT_EQUAL(cosim_test.getEventPipeline(core_id, hart_id)->getNumCached(), 0);

    // Final validation
    auto validate_final_state = [&](PegasusCoSim& cosim)
    {
        auto state = cosim.getPegasusCore(core_id)->getPegasusState(hart_id);
        auto sim_state = state->getSimState();
        auto workload_exit_code = sim_state->workload_exit_code;

        EXPECT_EQUAL(workload_exit_code, 0);
    };

    validate_final_state(cosim_truth);
    validate_final_state(cosim_test);
    EXPECT_EQUAL(exception_str, "");

    if (ERROR_CODE)
    {
        std::cout << "Test failed after " << step_count << " steps." << std::endl;
    }

    REPORT_ERROR;
    return ERROR_CODE;
}
