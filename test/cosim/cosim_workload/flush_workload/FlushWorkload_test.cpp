#include "cosim/PegasusCoSim.hpp"
#include "cosim/CoSimEventPipeline.hpp"
#include "simdb/apps/AppManager.hpp"
#include "sparta/kernel/SleeperThread.hpp"
#include "sparta/utils/SpartaTester.hpp"
#include <filesystem>
#include <regex>

/// In this test, we will be running the same workload through two PegasusCoSim
/// instances. One will ONLY step forward and serve as the "truth" against which
/// we will validate registers, PC, and other state. The other instance will step
/// forward N times, then flush to the N-1 instruction. Then the comparison takes
/// place, and we continue until the end of the workload.

using namespace pegasus;
using namespace pegasus::cosim;

std::string GetArchFromPath(const std::string & path)
{
    // Get the filename only (handles both absolute and relative paths)
    std::filesystem::path p(path);
    std::string filename = p.filename().string();

    // Simple regex: match "rv32" or "rv64" at the start of the filename
    std::regex rv_pattern(R"(^(rv(32|64)))");

    std::smatch match;
    if (std::regex_search(filename, match, rv_pattern))
    {
        return match.str(1); // "rv32" or "rv64"
    }

    // Optional: fall back to searching anywhere in the string
    std::regex anywhere(R"(rv(32|64))");
    if (std::regex_search(path, match, anywhere))
    {
        return match.str(0);
    }

    return "unknown";
}

bool StepSim(PegasusCoSim & sim, CoreId core_id, HartId hart_id)
{
    auto state = sim.getPegasusCore(core_id)->getPegasusState(hart_id);

    // Return false if we cannot step forward (end of sim)
    if (state->getSimState()->sim_stopped)
    {
        return false;
    }

    auto event = sim.step(core_id, hart_id);
    sim.commit(event);
    return true;
}

bool StepSimWithFlush(PegasusCoSim & sim, CoreId core_id, HartId hart_id,
                      size_t max_steps_before_flush = 3)
{
    auto state = sim.getPegasusCore(core_id)->getPegasusState(hart_id);

    // Return false if we cannot step forward (end of sim)
    if (state->getSimState()->sim_stopped)
    {
        return false;
    }

    // A value of 1 for max_steps_before_flush means:
    //   - step 1 time
    //   - flush that event
    //   - step 1 time again
    //   - commit that event
    if (max_steps_before_flush == 0)
    {
        throw std::invalid_argument("max_steps_before_flush must be >= 1");
    }
    else if (max_steps_before_flush == 1)
    {
        auto event = sim.step(core_id, hart_id);
        constexpr bool flush_younger_only = false;
        sim.flush(event, flush_younger_only);
        event = sim.step(core_id, hart_id);
        sim.commit(event);
        return true;
    }

    // Step N times, flush N-1 events, and commit the 1st step.
    std::vector<EventAccessor> stepped_events;
    while (stepped_events.size() < max_steps_before_flush)
    {
        auto event = sim.step(core_id, hart_id);
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
    return true;
}

template <typename XLEN>
bool Compare(PegasusCoSim & sim_truth, PegasusCoSim & sim_test, CoreId core_id, HartId hart_id)
{
    auto state_truth = sim_truth.getPegasusCore(core_id)->getPegasusState(hart_id);
    auto state_test = sim_test.getPegasusCore(core_id)->getPegasusState(hart_id);

    // Compare PegasusState member variables
    EXPECT_EQUAL(state_truth->getXlen(), state_test->getXlen());
    EXPECT_EQUAL(state_truth->getStopSimOnWfi(), state_test->getStopSimOnWfi());
    EXPECT_EQUAL(state_truth->getPc(), state_test->getPc());
    EXPECT_EQUAL(state_truth->getPrivMode(), state_test->getPrivMode());
    EXPECT_EQUAL(state_truth->getLdstPrivMode(), state_test->getLdstPrivMode());
    EXPECT_EQUAL(state_truth->getVirtualMode(), state_test->getVirtualMode());

    // Compare PegasusCore member variables
    EXPECT_EQUAL(state_truth->getCore()->isCompressionEnabled(),
                 state_test->getCore()->isCompressionEnabled());
    EXPECT_EQUAL(state_truth->getCore()->hasHypervisor(), state_test->getCore()->hasHypervisor());
    EXPECT_EQUAL(state_truth->getCore()->getPcAlignment(), state_test->getCore()->getPcAlignment());
    EXPECT_EQUAL(state_truth->getCore()->getPcAlignmentMask(),
                 state_test->getCore()->getPcAlignmentMask());
    EXPECT_EQUAL(state_truth->getCore()->getMisaExtFieldValue<XLEN>(),
                 state_test->getCore()->getMisaExtFieldValue<XLEN>());

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
    auto compare_simple_regs = [](RegisterSet* rset_truth, RegisterSet* rset_test)
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

    auto compare_csr_regs = [](RegisterSet* rset_truth, RegisterSet* rset_test)
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

    const auto & mem_reads_truth = last_event_truth->getMemoryReads();
    const auto & mem_reads_test = last_event_test->getMemoryReads();
    EXPECT_EQUAL(mem_reads_truth, mem_reads_test);

    const auto & mem_writes_truth = last_event_truth->getMemoryWrites();
    const auto & mem_writes_test = last_event_test->getMemoryWrites();
    EXPECT_EQUAL(mem_writes_truth, mem_writes_test);

    auto mem_writes_n = std::min(mem_writes_truth.size(), mem_writes_test.size());
    for (size_t i = 0; i < mem_writes_n; ++i)
    {
        const auto & write_truth = mem_writes_truth[i];
        const auto paddr = write_truth.paddr;
        auto mem_value_truth = state_truth->readMemory<XLEN>(paddr);
        auto mem_value_test = state_test->readMemory<XLEN>(paddr);
        EXPECT_EQUAL(mem_value_truth, mem_value_test);
    }

    // Compare enabled extensions
    auto extensions_map_truth =
        state_truth->getCore()->getExtensionManager().getEnabledExtensions();
    auto extensions_map_test = state_test->getCore()->getExtensionManager().getEnabledExtensions();

    std::unordered_set<std::string> enabled_exts_truth;
    for (auto it = extensions_map_truth.begin(); it != extensions_map_truth.end(); ++it)
    {
        const auto & ext_name = it->first;
        const auto truth_ext_enabled = extensions_map_truth.isEnabled(ext_name);
        EXPECT_EQUAL(extensions_map_test.isEnabled(ext_name), truth_ext_enabled);
    }

    return ERROR_CODE == 0;
}

template <typename XLEN>
bool AdvanceAndCompare(PegasusCoSim & sim_truth, PegasusCoSim & sim_test, CoreId core_id,
                       HartId hart_id, size_t max_steps_before_flush)
{
    auto stepped_truth = StepSim(sim_truth, core_id, hart_id);
    auto stepped_test = max_steps_before_flush > 0
                            ? StepSimWithFlush(sim_test, core_id, hart_id, max_steps_before_flush)
                            : StepSim(sim_test, core_id, hart_id);

    EXPECT_EQUAL(stepped_truth, stepped_test);
    if (stepped_truth && stepped_test)
    {
        return Compare<XLEN>(sim_truth, sim_test, core_id, hart_id);
    }
    return false;
}

std::string GenerateUUID()
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    uint64_t part1 = dis(gen);
    uint64_t part2 = dis(gen);

    // Set UUID version (4) and variant (RFC 4122)
    part1 = (part1 & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    part2 = (part2 & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(8) << ((part1 >> 32) & 0xFFFFFFFFULL) << '-'
        << std::setw(4) << ((part1 >> 16) & 0xFFFFULL) << '-' << std::setw(4) << (part1 & 0xFFFFULL)
        << '-' << std::setw(4) << ((part2 >> 48) & 0xFFFFULL) << '-' << std::setw(12)
        << (part2 & 0xFFFFFFFFFFFFULL);

    return oss.str();
}

// Parse the workload and 'max steps before flush' from command line arguments.
// The test is either run like this for unit tests:
//   ./FlushWorkload_test -w <workload>
//
// Or via the RunArchTests.py script which provides the workload path
// and other parameters.
//   ./FlushWorkload_test --debug-dump-filename <fname> -p <isa_path> <isa> -w <workload>
//
// Or for manual debugging:
//   ./FlushWorkload_test -w <workload> [--max-steps-before-flush <steps>] [--fast-forward-steps
//   <steps>]
//   --> '--max-steps-before-flush' controls how many steps to take (N) before flushing (N-1)
//   --> '--fast-forward-steps' says how many steps to take before starting flush comparisons
std::tuple<std::string, size_t, size_t> ParseArgs(int argc, char** argv)
{
    if (argc == 1)
    {
        throw std::invalid_argument("Must supply workload");
    }

    std::string workload;
    size_t max_steps_before_flush = 3;
    size_t fast_forward_steps = 0;

    for (int i = 1; i < argc;)
    {
        std::string arg = argv[i];
        if (arg == "-w")
        {
            workload = argv[i + 1];
            i += 2;
            continue;
        }
        else if (arg == "--debug-dump-filename")
        {
            // Only used by the RunArchTests.py core ISA tests (Pegasus simulator, not cosim)
            i += 2;
            continue;
        }
        else if (arg == "-p")
        {
            // Only used by the RunArchTests.py core ISA tests (Pegasus simulator, not cosim)
            i += 3;
            continue;
        }
        else if (arg == "--max-steps-before-flush")
        {
            max_steps_before_flush = std::stoul(argv[i + 1]);
            i += 2;
            continue;
        }
        else if (arg == "--fast-forward-steps")
        {
            fast_forward_steps = std::stoul(argv[i + 1]);
            i += 2;
            continue;
        }
        else
        {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }

    if (workload.empty())
    {
        throw std::invalid_argument("Must supply workload");
    }

    return {workload, max_steps_before_flush, fast_forward_steps};
}

int main(int argc, char** argv)
{
    const auto [workload, max_steps_before_flush, fast_forward_steps] = ParseArgs(argc, argv);
    const auto arch = GetArchFromPath(workload);

    // Disable sleeper thread so we can run two simulations at once.
    sparta::SleeperThread::disableForever();

    const uint64_t ilimit = 0;
    const auto uuid = GenerateUUID();
    const auto db_truth = uuid + "_truth.db";
    const auto db_test = uuid + "_test.db";

    sparta::Scheduler scheduler_truth;
    PegasusCoSim cosim_truth(&scheduler_truth, ilimit, workload, db_truth);

    sparta::Scheduler scheduler_test;
    PegasusCoSim cosim_test(&scheduler_test, ilimit, workload, db_test);

    const pegasus::CoreId core_id = 0;
    const pegasus::HartId hart_id = 0;
    std::string exception_str;

    auto advance_and_compare =
        (arch == "rv32") ? AdvanceAndCompare<uint32_t> : AdvanceAndCompare<uint64_t>;

    size_t step_count = 0;
    try
    {
        while (true)
        {
            ++step_count;
            auto steps_before_flush = step_count <= fast_forward_steps ? 0 : max_steps_before_flush;
            if (!advance_and_compare(cosim_truth, cosim_test, core_id, hart_id, steps_before_flush))
            {
                if (ERROR_CODE)
                {
                    std::cout << "Mismatch detected at step " << step_count << std::endl;
                }
                break;
            }
        }
        std::cout << "Completed " << step_count << " steps." << std::endl;
    }
    catch (const std::exception & ex)
    {
        exception_str = ex.what();
        std::cout << "Exception caught on step " << step_count << ": " << exception_str
                  << std::endl;
    }

    // Shutdown pipelines
    cosim_truth.finish();
    cosim_test.finish();

    // Final validation
    auto validate_final_state = [&](PegasusCoSim & cosim_truth, PegasusCoSim & cosim_test)
    {
        auto state_truth = cosim_truth.getPegasusCore(core_id)->getPegasusState(hart_id);
        auto sim_state_truth = state_truth->getSimState();
        auto workload_exit_code_truth = sim_state_truth->workload_exit_code;
        auto test_passed_truth = sim_state_truth->test_passed;
        auto sim_stopped_truth = sim_state_truth->sim_stopped;
        auto inst_count_truth = sim_state_truth->inst_count;

        auto state_test = cosim_test.getPegasusCore(core_id)->getPegasusState(hart_id);
        auto sim_state_test = state_test->getSimState();
        auto workload_exit_code_test = sim_state_test->workload_exit_code;
        auto test_passed_test = sim_state_test->test_passed;
        auto sim_stopped_test = sim_state_test->sim_stopped;
        auto inst_count_test = sim_state_test->inst_count;

        EXPECT_EQUAL(workload_exit_code_truth, 0);
        EXPECT_EQUAL(workload_exit_code_test, 0);

        EXPECT_TRUE(test_passed_truth);
        EXPECT_TRUE(test_passed_test);

        EXPECT_EQUAL(inst_count_truth, inst_count_test);

        // No need to clutter the failures... sim would not have stopped
        // if there were already mismatches.
        if (ERROR_CODE == 0)
        {
            EXPECT_TRUE(sim_stopped_truth);
            EXPECT_TRUE(sim_stopped_test);
        }
    };

    validate_final_state(cosim_truth, cosim_test);
    EXPECT_EQUAL(exception_str, "");

    REPORT_ERROR;
    return ERROR_CODE;
}
