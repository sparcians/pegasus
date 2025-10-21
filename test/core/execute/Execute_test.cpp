#include "core/PegasusCore.hpp"
#include "system/PegasusSystem.hpp"
#include "system/SystemCallEmulator.hpp"
#include "sparta/simulation/RootTreeNode.hpp"
#include "sparta/simulation/ResourceFactory.hpp"
#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/SpartaTester.hpp"

class ExecuteTester
{
  public:
    ExecuteTester()
    {
        root_tn_.setClock(&clk_);

        // top.allocators
        allocators_tn_.reset(new pegasus::PegasusAllocators(&root_tn_));

        // top.system
        tns_to_delete_.emplace_back(
            new sparta::ResourceTreeNode(&root_tn_, "system", "Pegasus System", &system_factory_));

        // top.system_call_emulator
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            &root_tn_, "system_call_emulator", "System Call Emulator", &sys_call_factory_));

        // top.core0
        tns_to_delete_.emplace_back(
            new sparta::ResourceTreeNode(&root_tn_, "core0", "cores", 0, "Core", &core_factory_));
        sparta::TreeNode* core_tn = tns_to_delete_.back().get();

        root_tn_.enterConfiguring();
        root_tn_.enterFinalized();
        root_tn_.bindTreeEarly();
        root_tn_.bindTreeLate();
        root_tn_.validatePreRun();

        state_.reset(core_tn->getResourceAs<pegasus::PegasusCore*>()->getPegasusState());
        execute_.reset(state_->getExecuteUnit());
    }

    ~ExecuteTester() { root_tn_.enterTeardown(); }

  private:
    // Sparta components
    sparta::Scheduler scheduler_;
    sparta::Clock clk_{"clock", &scheduler_};
    sparta::RootTreeNode root_tn_;
    sparta::ResourceFactory<pegasus::PegasusCore, pegasus::PegasusCore::PegasusCoreParameters>
        core_factory_;
    sparta::ResourceFactory<pegasus::PegasusSystem, pegasus::PegasusSystem::PegasusSystemParameters>
        system_factory_;
    sparta::ResourceFactory<pegasus::SystemCallEmulator,
                            pegasus::SystemCallEmulator::SystemCallEmulatorParameters>
        sys_call_factory_;
    std::unique_ptr<pegasus::PegasusAllocators> allocators_tn_;
    std::vector<std::unique_ptr<sparta::ResourceTreeNode>> tns_to_delete_;

    // PegasusState
    std::unique_ptr<pegasus::PegasusState> state_;

    // Execute
    std::unique_ptr<pegasus::Execute> execute_;
};

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    ExecuteTester tester;

    REPORT_ERROR;
    return ERROR_CODE;
}
