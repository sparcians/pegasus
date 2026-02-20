#pragma once

#include <vector>
#include <string>
#include <cinttypes>

#include "sim/PegasusSimParameters.hpp"
#include "core/PegasusCore.hpp"
#include "core/SimListener.hpp"
#include "system/PegasusSystem.hpp"
#include "sparta/app/Simulation.hpp"
#include "system/SystemCallEmulator.hpp"

namespace pegasus
{
    class PegasusSim : public sparta::app::Simulation
    {
      public:
        PegasusSim(sparta::Scheduler* scheduler);

        ~PegasusSim();

        // Run the simulator
        void run(uint64_t run_time) override;

        // Step the simulator. Returns false if simulation already ended.
        bool step(CoreId core_id, HartId hart_id);

        PegasusCore* getPegasusCore(CoreId core_id = 0) const { return cores_.at(core_id); }

        PegasusSystem* getPegasusSystem() const { return system_; }

        void enableInteractiveMode();

        void setEOTMode(const std::string & eot_mode);

        void useSpikeFormatting();

        void endSimulation(int64_t exit_code);

        void addSimListener(SimListener* listener)
        {
            if (std::find(sim_listeners_.begin(), sim_listeners_.end(), listener)
                == sim_listeners_.end())
            {
                sim_listeners_.push_back(listener);
            }
        }

      private:
        void buildTree_() override;
        void configureTree_() override;
        void bindTree_() override;
        void registerSimDbApps_(simdb::AppRegistrations* app_registrations) override;
        void parameterizeSimDbApps_(simdb::AppManager* app_mgr) override;
        void postFinalizeFramework_() override;

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

        // Pegasus Core for each core
        std::map<CoreId, PegasusCore*> cores_;

        // Pegasus system
        PegasusSystem* system_ = nullptr;

        // Simulation callback listeners (buildTree_, etc.)
        std::vector<SimListener*> sim_listeners_;

        friend class PegasusCoSim;
    };
} // namespace pegasus
