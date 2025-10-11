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

            /*
            for (const auto & reg_override : reg_value_overrides_)
            {
                const bool MUST_EXIST = true;
                sparta::Register* reg = state->findRegister(reg_override.first, MUST_EXIST);
                const uint64_t new_reg_value = std::stoull(reg_override.second, nullptr, 0);
                uint64_t old_value = 0;
                if (state->getXlen() == 64)
                {
                    old_value = reg->dmiRead<uint64_t>();
                    reg->dmiWrite<uint64_t>(new_reg_value);
                }
                else
                {
                    old_value = reg->dmiRead<uint32_t>();
                    reg->dmiWrite<uint32_t>(new_reg_value);
                }
                std::cout << std::hex << std::showbase << std::setfill(' ');

                std::cout << "Setting " << std::quoted(reg->getName());
                if (reg->getAliases().size() != 0)
                {
                    std::cout << " aka " << reg->getAliases();
                }
                std::cout << " to " << HEX16(new_reg_value) << " from default " << HEX16(old_value)
                          << std::endl;

                std::cout << "Fields:";

                if (not reg->getFields().empty())
                {
                    uint64_t max_field_val_size = 0;
                    size_t max_field_name_size = 0;

                    // Order the fields based on bit settings
                    struct OrderFields
                    {
                        bool operator()(const sparta::RegisterBase::Field* lhs,
                                        const sparta::RegisterBase::Field* rhs) const
                        {
                            return (lhs->getLowBit() > rhs->getLowBit());
                        }
                    };

                    // Go through the fields twice -- once to get formatting, the second to actually
                    // print
                    std::set<sparta::RegisterBase::Field*, OrderFields> ordered_fields;
                    for (const auto & field : reg->getFields())
                    {
                        max_field_val_size = std::max(field->getNumBits(), max_field_val_size);
                        max_field_name_size =
                            std::max(field->getName().size(), max_field_name_size);
                        ordered_fields.insert(field);
                    }
                    max_field_val_size =
                        (max_field_val_size > 4) ? max_field_val_size / 4 : max_field_val_size;
                    max_field_val_size += 2;

                    for (const auto & field : ordered_fields)
                    {
                        std::cout << "\n\t" << std::setw(max_field_val_size) << std::right
                                  << std::dec << field->read() << " "
                                  << std::setw(max_field_name_size) << std::right
                                  << field->getName() << std::right << std::dec << " ["
                                  << std::setw(2) << field->getLowBit() << ":" << std::setw(2)
                                  << field->getHighBit() << "] "
                                  << (field->isReadOnly() ? "RO" : "RW") << " (" << field->getDesc()
                                  << ")";
                    }
                }
                else
                {
                    std::cout << " None";
                }
                std::cout << std::endl;
            }*/

            const auto workload_and_args = system_->getWorkloadAndArgs();
            if (false == workload_and_args.empty())
            {
                state->setupProgramStack(workload_and_args);
            }
        }
    }
} // namespace pegasus
