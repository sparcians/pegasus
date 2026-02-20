#include "sim/PegasusSim.hpp"
#include "include/ActionTags.hpp"
#include "include/gen/CSRFieldIdxs64.hpp"
#include <filesystem>

#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{
    PegasusSim::PegasusSim(sparta::Scheduler* scheduler) :
        sparta::app::Simulation("PegasusSim", scheduler)
    {
    }

    PegasusSim::~PegasusSim()
    {
        for (auto & [core_idx, core] : cores_)
        {
            for (auto & [hart_idx, thread] : core->getThreads())
            {
                thread->cleanup();
            }
        }

        getRoot()->enterTeardown();
    }

    void PegasusSim::run(uint64_t run_time)
    {
        for (auto & [core_idx, core] : cores_)
        {
            core->boot();
        }

        getSimulationConfiguration()->scheduler_exacting_run = true;
        getSimulationConfiguration()->scheduler_measure_run_time = false;
        auto start = std::chrono::system_clock::system_clock::now();
        sparta::app::Simulation::run(run_time);
        auto end = std::chrono::system_clock::system_clock::now();
        auto sim_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        // FIXME: Only run core 0, hart 0 for now
        const CoreId core_idx = 0;
        const HartId hart_idx = 0;
        const PegasusState* state = cores_.at(core_idx)->getPegasusState(hart_idx);
        const uint64_t inst_count = state->getSimState()->inst_count;
        std::locale::global(std::locale(""));
        std::cout.imbue(std::locale());
        std::cout.precision(12);
        std::cout << "Instructions executed: " << std::dec << inst_count << std::endl;
        std::cout << "Raw time (seconds): " << std::dec << (sim_time / 1000000.0) << std::endl;
        std::cout << "MIPS: " << std::dec << ((inst_count / (sim_time / 1000000.0)) / 1000000.0)
                  << std::endl;

        // TODO: mem usage, workload exit code
    }

    bool PegasusSim::step(CoreId core_id, HartId hart_id)
    {
        auto state = getPegasusCore(core_id)->getPegasusState(hart_id);

        // Return false if we cannot step forward (end of sim)
        if (state->getSimState()->sim_stopped)
        {
            return false;
        }

        ActionGroup* next_action_group = state->getFetchUnit()->getActionGroup();
        do
        {
            next_action_group = next_action_group->execute(state);
        } while (next_action_group && (next_action_group->hasTag(ActionTags::FETCH_TAG) == false));

        return true;
    }

    void PegasusSim::setEOTMode(const std::string & eot_mode)
    {
        if (eot_mode == "pass_fail")
        {
            sparta::notNull(system_)->enableEOTPassFailMode();
        }
        else
        {
            sparta_assert(false, "Unknown EOT mode: " << eot_mode << " expected pass_fail");
        }
    }

    void PegasusSim::enableInteractiveMode()
    {
        sparta_assert(!cores_.empty(), "Must call after bindTree_()");
        for (auto & [core_idx, core] : cores_)
        {
            for (auto & [hart_idx, thread] : core->getThreads())
            {
                thread->enableInteractiveMode();
            }
        }
    }

    void PegasusSim::useSpikeFormatting()
    {
        sparta_assert(!cores_.empty(), "Must call after bindTree_()");
        for (auto & [core_idx, core] : cores_)
        {
            for (auto & [hart_idx, thread] : core->getThreads())
            {
                thread->useSpikeFormatting();
            }
        }
    }

    void PegasusSim::buildTree_()
    {
        for (auto listener : sim_listeners_)
        {
            listener->onBindTreeEarly();
        }

        auto root_tn = getRoot();

        // top.allocators
        allocators_tn_.reset(new PegasusAllocators(getRoot()));

        // top.system
        tns_to_delete_.emplace_back(
            new sparta::ResourceTreeNode(root_tn, "system", "Pegasus System", &system_factory_));

        // top.system_call_emulator
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            root_tn, "system_call_emulator", "System Call Emulator", &sys_call_factory_));

        getRoot()->addExtensionFactory(PegasusSimParameters::name,
                                       [&]() -> sparta::TreeNode::ExtensionsBase*
                                       { return new PegasusSimParameters(); });

        for (auto listener : sim_listeners_)
        {
            listener->onBindTreeLate();
        }
    }

    void PegasusSim::configureTree_()
    {
        for (auto listener : sim_listeners_)
        {
            listener->onConfigureTreeEarly();
        }

        // Get number of cores
        const uint32_t num_cores =
            PegasusSimParameters::getParameter<uint32_t>(getRoot(), "num_cores");

        // top.core*
        for (CoreId core_idx = 0; core_idx < num_cores; ++core_idx)
        {
            sparta::TreeNode* core_tn = nullptr;
            const std::string core_name = "core" + std::to_string(core_idx);
            tns_to_delete_.emplace_back(
                core_tn = new sparta::ResourceTreeNode(getRoot(), core_name, "cores", core_idx,
                                                       "Core", &core_factory_));
        }

        for (auto listener : sim_listeners_)
        {
            listener->onConfigureTreeLate();
        }
    }

    void PegasusSim::bindTree_()
    {
        for (auto listener : sim_listeners_)
        {
            listener->onBindTreeEarly();
        }

        // Pegasus System (shared by all cores)
        system_ = getRoot()->getChild("system")->getResourceAs<pegasus::PegasusSystem>();

        const uint32_t num_cores =
            PegasusSimParameters::getParameter<uint32_t>(getRoot(), "num_cores");
        for (CoreId core_idx = 0; core_idx < num_cores; ++core_idx)
        {
            const std::string core_name = "core" + std::to_string(core_idx);
            PegasusCore* core = getRoot()->getChild(core_name)->getResourceAs<PegasusCore*>();
            cores_.emplace(core_idx, core);
        }

        const auto & reg_overrides =
            PegasusSimParameters::getParameter<PegasusSimParameters::RegisterOverrides>(
                getRoot(), "reg_overrides");
        for (const auto & reg_override : reg_overrides)
        {
            sparta_assert(reg_override.size() == 2, "Register override param is malformed!");
            std::vector<std::string> tmp;
            const std::string delim = ".";
            sparta::utils::tokenize(reg_override[0], tmp, delim);
            sparta_assert(tmp.size() == 3, "Register override param is malformed!");
            const std::string & core = tmp[0];
            const std::string & hart = tmp[1];
            const std::string & reg_name = tmp[2];

            // Figure out which cores will receive this override
            std::vector<CoreId> core_ids;
            const std::string core_num = core.substr(4);
            if (core_num == "*")
            {
                core_ids.resize(num_cores);
                std::iota(core_ids.begin(), core_ids.end(), 0);
            }
            else
            {
                core_ids.emplace_back(std::stoi(core_num));
            }

            for (CoreId core_id : core_ids)
            {
                PegasusCore* pegasus_core = getPegasusCore(core_id);

                // Figure out which threads on this core will receive this override
                std::vector<HartId> hart_ids;
                const std::string hart_num = hart.substr(4);
                if (hart_num == "*")
                {
                    hart_ids.resize(pegasus_core->getNumThreads());
                    std::iota(hart_ids.begin(), hart_ids.end(), 0);
                }
                else
                {
                    hart_ids.emplace_back(std::stoi(hart_num));
                }

                for (HartId hart_id : hart_ids)
                {
                    PegasusState* thread = getPegasusCore(core_id)->getPegasusState(hart_id);

                    const bool MUST_EXIST = true;
                    sparta::Register* reg = thread->findRegister(reg_name, MUST_EXIST);
                    const uint64_t new_reg_value = std::stoull(reg_override[1], nullptr, 0);
                    uint64_t old_value = 0;
                    if (thread->getXlen() == 64)
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

                    std::cout << "Setting ";
                    std::cout << "\"core" << std::to_string(core_id) << ".hart"
                              << std::to_string(hart_id) << "." << reg->getName() << "\"";
                    if (reg->getAliases().size() != 0)
                    {
                        std::cout << " aka " << reg->getAliases();
                    }
                    std::cout << " to " << HEX16(new_reg_value) << " from default "
                              << HEX16(old_value) << std::endl;

                    if (not reg->getFields().empty())
                    {
                        std::cout << "Fields:";
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

                        // Go through the fields twice -- once to get formatting, the second to
                        // actually print
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
                            std::cout
                                << "\n\t" << std::setw(max_field_val_size) << std::right << std::dec
                                << field->read() << " " << std::setw(max_field_name_size)
                                << std::right << field->getName() << std::right << std::dec << " ["
                                << std::setw(2) << field->getLowBit() << ":" << std::setw(2)
                                << field->getHighBit() << "] "
                                << (field->isReadOnly() ? "RO" : "RW") << " (" << field->getDesc()
                                << ")";
                        }
                        std::cout << std::endl;
                    }
                }
            }
        }

        for (auto listener : sim_listeners_)
        {
            listener->onBindTreeLate();
        }
    }

    void PegasusSim::registerSimDbApps_(simdb::AppRegistrations* app_registrations)
    {
        for (auto listener : sim_listeners_)
        {
            listener->onRegisterAppsRequest(app_registrations);
        }
    }

    void PegasusSim::parameterizeSimDbApps_(simdb::AppManager* app_mgr)
    {
        for (auto listener : sim_listeners_)
        {
            listener->onParameterizeAppsRequest(app_mgr);
        }
    }

    void PegasusSim::postFinalizeFramework_()
    {
        for (auto listener : sim_listeners_)
        {
            listener->onFrameworkFinalized();
        }
    }

    void PegasusSim::endSimulation(int64_t exit_code)
    {
        for (auto & [core_id, core] : cores_)
        {
            core->stopSim(exit_code);
        }
    }

} // namespace pegasus
