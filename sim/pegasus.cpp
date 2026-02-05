
#include <iomanip>
#include <filesystem>

#include "sim/PegasusSim.hpp"
#include "sparta/app/CommandLineSimulator.hpp"

const char USAGE[] = "Usage:\n"
                     "./pegasus [-i inst limit] [--reg \"core*.hart*.name value\"] [--opcode "
                     "opcode] [--interactive] "
                     "--load-binary \"binary load_addr\"]"
                     "[--spike-formatting] <workloads>"
                     "\n";

using StringPairParam = std::pair<std::string, std::string>;

namespace std
{
    inline std::ostream & operator<<(std::ostream & os, StringPairParam const & p)
    {
        return os << "StringPairParam {" << p.first << ", " << p.second << "}";
    }

    static inline std::istream & operator>>(std::istream & is, StringPairParam & into)
    {
        return is >> std::skipws >> into.first >> into.second;
    }
}

int main(int argc, char** argv)
{
    uint64_t ilimit = 0;
    std::string opcode = "";
    std::vector<std::string> workloads;
    std::string eot_mode;

    sparta::app::DefaultValues DEFAULTS;
    DEFAULTS.auto_summary_default = "off";
    DEFAULTS.arch_arg_default = "rva23";
    DEFAULTS.arch_search_dirs = {"arch"}; // Where --arch will be resolved by default

    sparta::SimulationInfo::getInstance() =
        sparta::SimulationInfo("Pegasus RISC-V Functional Model", argc, argv, "v0.0.0", "", {});
    const bool show_field_names = true;
    sparta::SimulationInfo::getInstance().write(std::cout, "# ", "\n", show_field_names);
    std::cout << "# Sparta Version: " << sparta::SimulationInfo::sparta_version << std::endl;

    int exit_code = 0;
    try
    {
        // Helper class for parsing command line arguments, setting up the
        // simulator, and running the simulator. All of the things done by this
        // classs can be done manually if desired. Use the source for the
        // CommandLineSimulator class as a starting point
        sparta::app::CommandLineSimulator cls(USAGE, DEFAULTS);
        namespace po = boost::program_options;

        auto & app_opts = cls.getApplicationOptions();

        // clang-format off
        app_opts.add_options()
            ("inst-limit,i", po::value<uint64_t>(&ilimit),
             "Stop simulation after the instruction limit has been reached")
            ("reg", po::value<std::vector<StringPairParam>>()->multitoken(), "Override initial value of a register e.g. \"core0.hart0.sp 0x1000\"")
            ("opcode", po::value<std::string>(&opcode), "Executes a single opcode")
            ("interactive", "Enable interactive mode (IDE)")
            ("load-binary", po::value<std::vector<StringPairParam>>()->multitoken(), "Binary to load into memory at the specified address e.g. \"example.bin 0x80000000\"")
            ("eot-mode", po::value<std::string>(&eot_mode), "End of testing mode (pass_fail, magic_mem) [currently IGNORED]")
            ("spike-formatting", "Format the Instruction Logger similar to Spike")
            ("workloads,w", po::value<std::vector<std::string>>(&workloads), "Workload(s) to run with workload arguments");

        // Add any positional command-line options
        po::positional_options_description & pos_opts = cls.getPositionalOptions();
        pos_opts.add("workloads", -1);
        // clang-format on

        // Parse command line options and configure simulator
        int err_code = 0;
        if (!cls.parse(argc, argv, err_code))
        {
            return err_code; // Any errors already printed to cerr
        }

        const auto & vm = cls.getVariablesMap();

        if (vm.count("opcode"))
        {
            ilimit = 1;
        }

        // Process command line parameters
        auto & sim_cfg = cls.getSimulationConfiguration();

        // Workload
        pegasus::PegasusSimParameters::WorkloadsAndArgs workloads_and_args;
        if (workloads.empty() == false)
        {
            for (uint32_t wkld_idx = 0; wkld_idx < workloads.size(); ++wkld_idx)
            {
                workloads_and_args.emplace_back();
                pegasus::PegasusSimParameters::WorkloadAndArgs & workload_and_args =
                    workloads_and_args.back();
                sparta::utils::tokenize_on_whitespace(workloads[wkld_idx], workload_and_args);

                // Get full path of workload
                const std::filesystem::path workload_path =
                    std::filesystem::canonical(std::filesystem::absolute(workload_and_args[0]));

                workload_and_args[0] = workload_path.string();
            }

            const std::string wkld_and_args_param_value =
                pegasus::PegasusSimParameters::convertVectorToStringParam(workloads_and_args);
            sim_cfg.processParameter("top.extension.sim.workloads", wkld_and_args_param_value);
        }

        // Inst limit
        if (ilimit != 0)
        {
            sim_cfg.processParameter("top.extension.sim.inst_limit", std::to_string(ilimit));
        }

        // Register overrides
        if (vm.count("reg"))
        {
            std::string reg_overrides_param_value = "[";
            const auto reg_overrides = vm["reg"].as<std::vector<StringPairParam>>();
            for (uint32_t reg_idx = 0; reg_idx < reg_overrides.size(); ++reg_idx)
            {
                const auto & reg_override = reg_overrides[reg_idx];
                reg_overrides_param_value +=
                    "[" + reg_override.first + ", " + reg_override.second + "]";
                const bool last_overrides = reg_idx == (reg_overrides.size() - 1);
                reg_overrides_param_value += last_overrides ? "" : ",";
            }

            reg_overrides_param_value += "]";
            sim_cfg.processParameter("top.extension.sim.reg_overrides", reg_overrides_param_value);
        }

        // Register overrides
        if (vm.count("load-binary"))
        {
            std::string load_binaries_param_value = "[";
            const auto load_binaries = vm["load-binary"].as<std::vector<StringPairParam>>();
            for (uint32_t reg_idx = 0; reg_idx < load_binaries.size(); ++reg_idx)
            {
                const auto & reg_override = load_binaries[reg_idx];
                load_binaries_param_value +=
                    "[" + reg_override.first + ", " + reg_override.second + "]";
                const bool last_overrides = reg_idx == (load_binaries.size() - 1);
                load_binaries_param_value += last_overrides ? "" : ",";
            }

            load_binaries_param_value += "]";
            sim_cfg.processParameter("top.extension.sim.load_binaries", load_binaries_param_value);
        }

        // Create the simulator
        sparta::Scheduler scheduler;
        pegasus::PegasusSim sim(&scheduler);

        cls.populateSimulation(&sim);

        if (vm.count("opcode"))
        {
            const pegasus::CoreId core_idx = 0;
            const pegasus::HartId hart_idx = 0;
            const uint64_t pc = 0x1000;
            // Assume opcode is a hex string
            const uint64_t opcode_val = std::stoull(opcode, nullptr, 16);
            pegasus::PegasusState* state = sim.getPegasusCore(core_idx)->getPegasusState(hart_idx);
            state->writeMemory(pc, opcode_val);
            state->setPc(pc);
        }

        if (vm.count("interactive"))
        {
            sim.enableInteractiveMode();
        }

        if (vm.count("spike-formatting") > 0)
        {
            sim.useSpikeFormatting();
        }

        if (not eot_mode.empty())
        {
            sim.setEOTMode(eot_mode);
        }

        cls.runSimulator(&sim);

        cls.postProcess(&sim);

        // Get workload exit code
        const pegasus::CoreId core_idx = 0;
        const pegasus::HartId hart_idx = 0;
        const pegasus::PegasusState::SimState* sim_state =
            sim.getPegasusCore(core_idx)->getPegasusState(hart_idx)->getSimState();
        exit_code = sim_state->workload_exit_code;
        std::cout << "Workload exit code: " << std::dec << exit_code << std::endl;
    }
    catch (...)
    {
        // Could still handle or log the exception here
        throw;
    }

    return exit_code;
}
