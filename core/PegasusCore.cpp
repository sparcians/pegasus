#include "PegasusCore.hpp"
#include "system/PegasusSystem.hpp"
#include "system/SystemCallEmulator.hpp"

#include "sparta/simulation/ResourceTreeNode.hpp"

namespace pegasus
{
    PegasusCore::PegasusCore(sparta::TreeNode* core_tn, const PegasusCoreParameters* p) :
        sparta::Unit(core_tn),
        core_id_(p->core_id),
        num_harts_(p->num_harts)
    {
        // top.core*.hart*
        for (HartId hart_idx = 0; hart_idx < num_harts_; ++hart_idx)
        {
            sparta::TreeNode* hart_tn = nullptr;
            const std::string hart_name = "hart" + std::to_string(hart_idx);
            tns_to_delete_.emplace_back(
                hart_tn = new sparta::ResourceTreeNode(core_tn, hart_name, "harts", hart_idx,
                                                       "Hart State", &state_factory_));

            // top.core*.hart*.fetch
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "fetch", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Fetch Unit", &fetch_factory_));

            // top.core*.hart*.translate
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "translate", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Translate Unit", &translate_factory_));

            // top.core*.hart*.execute
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "execute", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Execute Unit", &execute_factory_));

            // top.core*.hart*.exception
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "exception", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Exception Unit", &exception_factory_));
        }
    }

    PegasusCore::~PegasusCore()
    {
        for (auto & [hart_idx, thread] : threads_)
        {
            thread->cleanup();
        }
    }

    void PegasusCore::boot()
    {
        for (auto & [hart_idx, thread] : threads_)
        {
            thread->boot();
        }
    }

    void PegasusCore::stopSim(const int64_t exit_code)
    {
        for (auto & [hart_idx, thread] : threads_)
        {
            thread->stopSim(exit_code);
        }
    }

    /*void PegasusCore::onConfiguring_()
    {
        // Set instruction limit for stopping simulation
        if (ilimit_ > 0)
        {
            auto core_ilimit_arg =
                getRoot()->getChildAs<sparta::ParameterBase>("core0.params.ilimit");
            core_ilimit_arg->setValueFromString(std::to_string(ilimit_));
        }
    }*/

    void PegasusCore::onBindTreeLate_()
    {
        // Pegasus System (shared by all harts)
        auto root_tn = getContainer()->getParentAs<sparta::RootTreeNode>();
        system_ = root_tn->getChild("system")->getResourceAs<pegasus::PegasusSystem>();
        SystemCallEmulator* system_call_emulator =
            root_tn->getChild("system_call_emulator")->getResourceAs<pegasus::SystemCallEmulator>();

        for (uint32_t hart_idx = 0; hart_idx < num_harts_; ++hart_idx)
        {
            // Get PegasusState and Fetch for each hart
            const std::string hart_name = "hart" + std::to_string(hart_idx);
            const auto thread = getContainer()->getChild(hart_name);
            threads_.emplace(hart_idx, thread->getResourceAs<PegasusState*>());

            // Give PegasusState a pointer to PegasusSystem for accessing memory
            PegasusState* state = threads_.at(hart_idx);
            state->setPegasusSystem(system_);
            state->setPc(system_->getStartingPc());

            if (thread->getChildAs<sparta::ResourceTreeNode>("execute")
                    ->getParameterSet()
                    ->getParameterAs<bool>("enable_syscall_emulation"))
            {
                state->setSystemCallEmulator(system_call_emulator);
            }

            const auto workload_and_args = system_->getWorkloadAndArgs();
            if (false == workload_and_args.empty())
            {
                state->setupProgramStack(workload_and_args);
            }
        }
    }
} // namespace pegasus
