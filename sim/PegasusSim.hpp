#pragma once

#include <vector>
#include <string>
#include <cinttypes>

#include "core/PegasusCore.hpp"
#include "system/PegasusSystem.hpp"
#include "sparta/app/Simulation.hpp"
#include "system/SystemCallEmulator.hpp"

namespace pegasus
{
    class PegasusSim : public sparta::app::Simulation
    {
      public:
        using RegValueOverridePairs = std::vector<std::pair<std::string, std::string>>;
        using WorkloadAndArguments = std::vector<std::string>;

        PegasusSim(sparta::Scheduler* scheduler, const WorkloadAndArguments & workload_and_args,
                   const RegValueOverridePairs & reg_value_overrides, uint64_t ilimit);
        ~PegasusSim();

        // Run the simulator
        void run(uint64_t run_time) override;

        PegasusCore* getPegasusCore(CoreId core_id = 0) const { return cores_.at(core_id); }

        PegasusSystem* getPegasusSystem() const { return system_; }

        void enableInteractiveMode();

        void setEOTMode(const std::string & eot_mode);

        void useSpikeFormatting();

        void endSimulation(int64_t exit_code);

      private:
        void buildTree_() override;
        void configureTree_() override;
        void bindTree_() override;

        sparta::ResourceFactory<pegasus::PegasusCore, pegasus::PegasusCore::PegasusCoreParameters>
            core_factory_;
        sparta::ResourceFactory<pegasus::PegasusSystem,
                                pegasus::PegasusSystem::PegasusSystemParameters>
            system_factory_;
        sparta::ResourceFactory<pegasus::SystemCallEmulator,
                                pegasus::SystemCallEmulator::SystemCallEmulatorParameters>
            sys_call_factory_;
        std::unique_ptr<PegasusAllocators> allocators_tn_;
        std::vector<std::unique_ptr<sparta::TreeNode>> tns_to_delete_;

        // TODO: Make this a parameter
        const uint32_t num_cores_ = 1;

        // Pegasus Core for each core
        std::map<CoreId, PegasusCore*> cores_;

        // Pegasus system
        PegasusSystem* system_ = nullptr;

        const WorkloadAndArguments workload_and_args_;
        const RegValueOverridePairs reg_value_overrides_;
        const uint64_t ilimit_;

        friend class PegasusCoSim;
    };
} // namespace pegasus
