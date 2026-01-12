#include "cosim/PegasusCoSim.hpp"
#include "sim/PegasusSim.hpp"
#include "cosim/CoSimEventPipeline.hpp"
#include "core/observers/InstructionLogger.hpp"
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

using pegasus::CoreId;
using pegasus::HartId;
using pegasus::PegasusSim;
using pegasus::RegisterSet;
using pegasus::cosim::EventAccessor;
using pegasus::cosim::PegasusCoSim;

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

bool StepSim(PegasusSim & sim, CoreId core_id, HartId hart_id)
{
    return sim.step(core_id, hart_id);
}

bool StepSim(PegasusCoSim & sim, CoreId core_id, HartId hart_id)
{
    auto state = sim.getPegasusSim().getPegasusCore(core_id)->getPegasusState(hart_id);

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
    auto state = sim.getPegasusSim().getPegasusCore(core_id)->getPegasusState(hart_id);

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
bool Compare(PegasusSim & sim_truth, PegasusCoSim & sim_test, CoreId core_id, HartId hart_id)
{
    auto state_truth = sim_truth.getPegasusCore(core_id)->getPegasusState(hart_id);
    auto state_test = sim_test.getPegasusSim().getPegasusCore(core_id)->getPegasusState(hart_id);

    // Compare PegasusState
    state_truth->compare<true>(state_test);

    // Compare SimState
    auto sim_state_truth = state_truth->getSimState();
    auto sim_state_test = state_test->getSimState();
    sim_state_truth->compare<true>(sim_state_test);

    // Compare PegasusCore
    auto core_truth = state_truth->getCore();
    auto core_test = state_test->getCore();
    core_truth->compare<true>(core_test);

    // Compare current inst
    auto inst_truth = state_truth->getCurrentInst();
    auto inst_test = state_test->getCurrentInst();
    if (inst_truth && inst_test)
    {
        inst_truth->compare<true>(inst_test);
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
    vec_config_truth->compare<true>(vec_config_test);

    // Compare memory
    auto inst_logger =
        dynamic_cast<const pegasus::InstructionLogger*>(state_truth->getObservers()[0].get());
    const auto & mem_writes_truth = inst_logger->getMemoryWrites();
    for (const auto & mem_write : mem_writes_truth)
    {
        auto mem_value_truth = state_truth->readMemory<uint64_t>(mem_write.addr);
        auto mem_value_test = state_test->readMemory<uint64_t>(mem_write.addr);
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
bool AdvanceAndCompare(PegasusSim & sim_truth, PegasusCoSim & sim_test, CoreId core_id,
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
//   <steps>] [--db-stem <stem>]
//   --> '--max-steps-before-flush' controls how many steps to take (N) before flushing (N-1)
//   --> '--fast-forward-steps' says how many steps to take before starting flush comparisons
//   --> '--db-stem' specifies the database stem name
std::tuple<std::string, std::string, size_t, size_t> ParseArgs(int argc, char** argv)
{
    if (argc == 1)
    {
        throw std::invalid_argument("Must supply workload");
    }

    std::string workload;
    std::string db_stem;
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
        else if (arg == "--db-stem")
        {
            db_stem = argv[i + 1];
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

    return {workload, db_stem, max_steps_before_flush, fast_forward_steps};
}

int main(int argc, char** argv)
{
    const auto [workload, db_stem, max_steps_before_flush, fast_forward_steps] =
        ParseArgs(argc, argv);
    const auto arch = GetArchFromPath(workload);

    // Disable sleeper thread so we can run two simulations at once.
    sparta::SleeperThread::disableForever();

    const uint64_t ilimit = 0;

    const auto cwd = std::filesystem::current_path().string();
    const auto workload_fname =
        !db_stem.empty() ? db_stem : std::filesystem::path(workload).filename().string();
    const auto db_truth = cwd + "/" + workload_fname + "_truth.db";
    const auto db_test = cwd + "/" + workload_fname + "_test.db";

    const size_t snapshot_threshold = 10;

    sparta::app::SimulationConfiguration config_truth;
    config_truth.enableLogging("top", "inst", workload_fname + ".log");
    config_truth.processParameter("top.core0.params.isa", "rv64gcbv_zicsr_zifencei_zicond_zfh",
                                  false);
    pegasus::PegasusSimParameters::WorkloadsAndArgs workloads_and_args{{workload}};
    const std::string wkld_param =
        pegasus::PegasusSimParameters::convertVectorToStringParam(workloads_and_args);
    config_truth.processParameter("top.extension.sim.workloads", wkld_param);
    config_truth.processParameter("top.extension.sim.inst_limit", std::to_string(ilimit));
    config_truth.copyTreeNodeExtensionsFromArchAndConfigPTrees();

    sparta::Scheduler scheduler_truth;
    PegasusSim cosim_truth(&scheduler_truth);

    cosim_truth.configure(0, nullptr, &config_truth);
    cosim_truth.buildTree();
    cosim_truth.configureTree();
    cosim_truth.finalizeTree();
    cosim_truth.finalizeFramework();

    // Assume 1 core, 1 hart for now
    const pegasus::CoreId num_cores = 1;
    const pegasus::HartId num_harts = 1;
    for (pegasus::CoreId core_id = 0; core_id < num_cores; ++core_id)
    {
        for (pegasus::HartId hart_id = 0; hart_id < num_harts; ++hart_id)
        {
            auto state = cosim_truth.getPegasusCore(core_id)->getPegasusState(hart_id);
            state->boot();
        }
    }

    const std::map<std::string, std::string> params = {
        {"top.core*.params.isa", "rv64gcbv_zicsr_zifencei_zicond_zfh"}};
    PegasusCoSim cosim_test(ilimit, workload, params, db_test, snapshot_threshold);

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
    cosim_test.finish();

    // Final validation
    auto validate_final_state = [&](PegasusSim & cosim_truth, PegasusCoSim & cosim_test)
    {
        auto state_truth = cosim_truth.getPegasusCore(core_id)->getPegasusState(hart_id);
        auto sim_state_truth = state_truth->getSimState();
        auto workload_exit_code_truth = sim_state_truth->workload_exit_code;
        auto test_passed_truth = sim_state_truth->test_passed;
        auto sim_stopped_truth = sim_state_truth->sim_stopped;
        auto inst_count_truth = sim_state_truth->inst_count;

        auto state_test =
            cosim_test.getPegasusSim().getPegasusCore(core_id)->getPegasusState(hart_id);
        auto sim_state_test = state_test->getSimState();
        auto workload_exit_code_test = sim_state_test->workload_exit_code;
        auto test_passed_test = sim_state_test->test_passed;
        auto sim_stopped_test = sim_state_test->sim_stopped;
        auto inst_count_test = sim_state_test->inst_count;

        EXPECT_EQUAL(workload_exit_code_truth, workload_exit_code_test);
        EXPECT_EQUAL(test_passed_truth, test_passed_test);
        EXPECT_EQUAL(inst_count_truth, inst_count_test);
        EXPECT_EQUAL(sim_stopped_truth, sim_stopped_test);
    };

    validate_final_state(cosim_truth, cosim_test);
    EXPECT_EQUAL(exception_str, "");

    REPORT_ERROR;
    return ERROR_CODE;
}
