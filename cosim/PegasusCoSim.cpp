
#include "cosim/PegasusCoSim.hpp"
#include "sim/PegasusSim.hpp"
#include "core/Fetch.hpp"
#include "core/observers/CoSimObserver.hpp"
#include "include/ActionTags.hpp"

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/app/SimulationConfiguration.hpp"
#include "sparta/kernel/Scheduler.hpp"
#include "sparta/log/MessageSource.hpp"
#include "sparta/log/Tap.hpp"
#include "sparta/utils/SpartaAssert.hpp"
#include "sparta/functional/Register.hpp"

#include "cosim/CoSimEventPipeline.hpp"
#include "sparta/serialization/checkpoint/CherryPickFastCheckpointer.hpp"
#include "simdb/sqlite/DatabaseManager.hpp"
#include "simdb/apps/AppManager.hpp"

namespace pegasus::cosim
{
    CoSimMemoryInterface::CoSimMemoryInterface(sparta::memory::SimpleMemoryMapNode* memory) :
        memory_(memory)
    {
    }

    bool CoSimMemoryInterface::peek(CoreId, HartId, Addr paddr, size_t size,
                                    std::vector<uint8_t> & buffer) const
    {
        const bool success = memory_->tryPeek(paddr, size, buffer.data());
        return success;
    }

    bool CoSimMemoryInterface::read(CoreId, HartId, Addr paddr, size_t size,
                                    std::vector<uint8_t> & buffer) const
    {
        const bool success = memory_->tryRead(paddr, size, buffer.data());
        return success;
    }

    bool CoSimMemoryInterface::poke(CoreId, HartId, Addr paddr, std::vector<uint8_t> & buffer) const
    {
        const size_t size = buffer.size() * sizeof(uint8_t);
        const bool success = memory_->tryPoke(paddr, size, buffer.data());
        return success;
    }

    bool CoSimMemoryInterface::write(CoreId, HartId, Addr paddr,
                                     std::vector<uint8_t> & buffer) const
    {
        const size_t size = buffer.size() * sizeof(uint8_t);
        const bool success = memory_->tryWrite(paddr, size, buffer.data());
        return success;
    }

    PegasusCoSim::PegasusCoSim(uint64_t ilimit, const std::string & workload,
                               const std::map<std::string, std::string> pegasus_params,
                               const std::string & db_file, const size_t snapshot_threshold)
    {
        sim_config_.reset(new sparta::app::SimulationConfiguration);

        for (auto [param, value] : pegasus_params)
        {
            constexpr bool OPTIONAL = false;
            sim_config_->processParameter(param, value, OPTIONAL);
        }

        // Instruction count limit
        sim_config_->processParameter("top.extension.sim.inst_limit", std::to_string(ilimit));

        // Workload
        PegasusSimParameters::WorkloadsAndArgs workloads_and_args{{workload}};
        const std::string wkld_param =
            PegasusSimParameters::convertVectorToStringParam(workloads_and_args);
        sim_config_->processParameter("top.extension.sim.workloads", wkld_param);

        // Put all cores in CoSim mode
        sim_config_->processParameter("top.core*.params.cosim_mode", "true");

        sim_config_->copyTreeNodeExtensionsFromArchAndConfigPTrees();

        scheduler_.reset(new sparta::Scheduler());
        pegasus_sim_.reset(new PegasusSim(scheduler_.get()));
        cosim_logger_.reset(new sparta::log::MessageSource(pegasus_sim_->getRoot(), "cosim",
                                                           "Pegasus Cosim Logger"));

        pegasus_sim_->addSimListener(this);
        pegasus_sim_->configure(0, nullptr, sim_config_.get());
        pegasus_sim_->buildTree();
        pegasus_sim_->configureTree();
        pegasus_sim_->finalizeTree();

        // Now that the tree is built, we can access PegasusSimParameters (tree node extension)
        // and we can figure out the number of cores/harts (parameter in the PegasusCore unit).

        // Get number of cores
        const uint32_t num_cores =
            PegasusSimParameters::getParameter<uint32_t>(pegasus_sim_->getRoot(), "num_cores");
        cosim_observers_.resize(num_cores);

        // Get total number of harts across all cores
        uint32_t total_num_harts = 0;
        for (uint32_t core_idx = 0; core_idx < num_cores; core_idx++)
        {
            const std::string core_name = "core" + std::to_string(core_idx);
            const sparta::ParameterSet* core_params =
                pegasus_sim_->getRoot()
                    ->getChildAs<sparta::ResourceTreeNode*>(core_name)
                    ->getParameterSet();
            const uint32_t num_harts =
                core_params->getParameter("num_harts")->getValueAs<uint32_t>();

            num_harts_per_core_.emplace_back(num_harts);
            total_num_harts += num_harts;
            cosim_observers_.at(core_idx).resize(num_harts, nullptr);
        }

        // Since we are not using CommandLineSimulator to handle the SimulationConfiguration,
        // we need to manually make the calls that the CLS would have.
        auto & simdb_config = sim_config_->simdb_config;
        simdb_config.setSimExecutable("pegasus_cosim");
        simdb_config.enableApp(CoSimEventPipeline::NAME);
        simdb_config.enableApp(CoSimCheckpointer::NAME);
        simdb_config.setAppCount(CoSimEventPipeline::NAME, total_num_harts);
        simdb_config.setAppCount(CoSimCheckpointer::NAME, total_num_harts);
        simdb_config.setAppDatabase(CoSimEventPipeline::NAME, db_file);
        simdb_config.setAppDatabase(CoSimCheckpointer::NAME, db_file);

        // Finish setting up the simulator
        pegasus_sim_->finalizeFramework();

        fetch_.resize(num_cores);
        size_t pipeline_idx =
            (total_num_harts == 1) ? 0 : 1; // 0 if only one pipeline, else 1-based
        for (CoreId core_idx = 0; core_idx < num_cores; ++core_idx)
        {
            for (HartId hart_idx = 0; hart_idx < num_harts_per_core_.at(core_idx); ++hart_idx)
            {
                // Get Fetch for each hart
                const std::string core_name = "core" + std::to_string(core_idx) + ".";
                const std::string hart_name = "hart" + std::to_string(hart_idx) + ".";
                fetch_.at(core_idx).emplace_back(pegasus_sim_->getRoot()
                                                     ->getChild(core_name + hart_name + "fetch")
                                                     ->getResourceAs<pegasus::Fetch>());

                auto state = pegasus_sim_->getPegasusCore(core_idx)->getPegasusState(hart_idx);

                // Create and attach CoSimObserver to PegasusState for each hart
                auto evt_pipeline = app_mgr_->getAppInstance<CoSimEventPipeline>(pipeline_idx);
                auto checkpointer = app_mgr_->getAppInstance<CoSimCheckpointer>(pipeline_idx);
                ++pipeline_idx;

                auto cosim_obs = std::make_unique<CoSimObserver>(*cosim_logger_.get(), evt_pipeline,
                                                                 checkpointer, core_idx, hart_idx);

                evt_pipeline->setObserver(cosim_obs.get());

                // Initialize PegasusState and take initial snapshot
                state->boot();
                checkpointer->getFastCheckpointer().setSnapshotThreshold(snapshot_threshold);
                checkpointer->getFastCheckpointer().createHead();

                // Store observer
                cosim_observers_.at(core_idx).at(hart_idx) = cosim_obs.get();
                state->addObserver(std::move(cosim_obs));
            }
        }

        // Single memory IF for all harts
        cosim_memory_if_ =
            new CoSimMemoryInterface(pegasus_sim_->getPegasusSystem()->getSystemMemory());
    }

    PegasusCoSim::~PegasusCoSim() noexcept { pegasus_sim_->getRoot()->enterTeardown(); }

    CoSimEventPipeline* PegasusCoSim::getEventPipeline(CoreId core_id, HartId hart_id)
    {
        return cosim_observers_.at(core_id).at(hart_id)->getEventPipeline();
    }

    const CoSimEventPipeline* PegasusCoSim::getEventPipeline(CoreId core_id, HartId hart_id) const
    {
        return cosim_observers_.at(core_id).at(hart_id)->getEventPipeline();
    }

    void PegasusCoSim::enableLogger(const std::string & filename)
    {
        if (!filename.empty())
        {
            sparta_tap_.reset(new sparta::log::Tap(pegasus_sim_->getRoot(), "cosim", filename));
        }
        else
        {
            sparta_tap_.reset(new sparta::log::Tap(pegasus_sim_->getRoot(), "cosim", std::cout));
        }
    }

    void PegasusCoSim::logMessage(const std::string & message) { *cosim_logger_ << message; }

    EventAccessor PegasusCoSim::step(CoreId core_id, HartId hart_id)
    {
        ActionGroup* next_action_group = fetch_.at(core_id).at(hart_id)->getActionGroup();
        PegasusState* state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        do
        {
            next_action_group = next_action_group->execute(state);
        } while (next_action_group && (next_action_group->hasTag(ActionTags::FETCH_TAG) == false));

        auto evt_pipeline = getEventPipeline(core_id, hart_id);
        return evt_pipeline->getLastEvent();

#if 0
        event_list_.at(hart_id).emplace_back(event);
        return event_list_.at(hart_id).back();
#endif
    }

    EventAccessor PegasusCoSim::step(CoreId core_id, HartId hart_id, Addr addr)
    {
        setPc(core_id, hart_id, addr);
        return step(core_id, hart_id);
    }

    EventAccessor PegasusCoSim::stepOperation(CoreId, HartId)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    EventAccessor PegasusCoSim::stepOperation(CoreId, HartId, Addr)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::commit(CoreId core_id, HartId hart_id)
    {
        auto evt_pipeline = getEventPipeline(core_id, hart_id);
        evt_pipeline->commitOldest();
    }

    void PegasusCoSim::commit(cosim::EventAccessor & event)
    {
        auto core_id = event.getCoreId();
        auto hart_id = event.getHartId();
        auto evt_pipeline = getEventPipeline(core_id, hart_id);
        evt_pipeline->commitUpTo(event.getEuid());
    }

    void PegasusCoSim::commitStoreWrite(cosim::EventAccessor &)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::commitStoreWrite(cosim::EventAccessor &, Addr)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::dropStoreWrite(cosim::EventAccessor &)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::dropStoreWrite(cosim::EventAccessor &, Addr)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::flush(cosim::EventAccessor & event, bool flush_younger_only)
    {
        auto core_id = event.getCoreId();
        auto hart_id = event.getHartId();
        auto observer = cosim_observers_.at(core_id).at(hart_id);
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        auto evt_pipeline = getEventPipeline(core_id, hart_id);
        evt_pipeline->flush(event.getEuid(), flush_younger_only, observer, state);
    }

    cosim::MemoryInterface* PegasusCoSim::getMemoryInterface() { return cosim_memory_if_; }

    void PegasusCoSim::setMemoryInterface(cosim::MemoryInterface*)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::readRegister(CoreId core_id, HartId hart_id, RegId reg_id,
                                    std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        sparta::Register* reg = state->findRegister(reg_id);
        readRegister_(reg, buffer);
    }

    void PegasusCoSim::peekRegister(CoreId core_id, HartId hart_id, RegId reg_id,
                                    std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        sparta::Register* reg = state->findRegister(reg_id);
        peekRegister_(reg, buffer);
    }

    void PegasusCoSim::writeRegister(CoreId core_id, HartId hart_id, RegId reg_id,
                                     std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        sparta::Register* reg = state->findRegister(reg_id);
        writeRegister_(reg, buffer);
    }

    void PegasusCoSim::pokeRegister(CoreId core_id, HartId hart_id, RegId reg_id,
                                    std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        sparta::Register* reg = state->findRegister(reg_id);
        pokeRegister_(reg, buffer);
    }

    void PegasusCoSim::readRegister(CoreId core_id, HartId hart_id, const std::string reg_name,
                                    std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        constexpr bool MUST_EXIST = true;
        sparta::Register* reg = state->findRegister(reg_name, MUST_EXIST);
        readRegister_(reg, buffer);
    }

    void PegasusCoSim::peekRegister(CoreId core_id, HartId hart_id, const std::string reg_name,
                                    std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        constexpr bool MUST_EXIST = true;
        sparta::Register* reg = state->findRegister(reg_name, MUST_EXIST);
        peekRegister_(reg, buffer);
    }

    void PegasusCoSim::writeRegister(CoreId core_id, HartId hart_id, const std::string reg_name,
                                     std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        constexpr bool MUST_EXIST = true;
        sparta::Register* reg = state->findRegister(reg_name, MUST_EXIST);
        writeRegister_(reg, buffer);
    }

    void PegasusCoSim::pokeRegister(CoreId core_id, HartId hart_id, const std::string reg_name,
                                    std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        constexpr bool MUST_EXIST = true;
        sparta::Register* reg = state->findRegister(reg_name, MUST_EXIST);
        pokeRegister_(reg, buffer);
    }

    void PegasusCoSim::readRegisterField(CoreId core_id, HartId hart_id, const std::string reg_name,
                                         const std::string field_name,
                                         std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        constexpr bool MUST_EXIST = true;
        sparta::Register::Field* field =
            state->findRegister(reg_name, MUST_EXIST)->getField(field_name);
        // All field accesses are 64 bit
        const uint64_t value = field->read();
        buffer.resize(sizeof(uint64_t));
        std::memcpy(buffer.data(), &value, sizeof(uint64_t));
    }

    void PegasusCoSim::peekRegisterField(CoreId core_id, HartId hart_id, const std::string reg_name,
                                         const std::string field_name,
                                         std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        constexpr bool MUST_EXIST = true;
        sparta::Register::Field* field =
            state->findRegister(reg_name, MUST_EXIST)->getField(field_name);
        // All field accesses are 64 bit
        const uint64_t value = field->peek();
        buffer.resize(sizeof(uint64_t));
        std::memcpy(buffer.data(), &value, sizeof(uint64_t));
    }

    void PegasusCoSim::writeRegisterField(CoreId core_id, HartId hart_id,
                                          const std::string reg_name, const std::string field_name,
                                          std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        constexpr bool MUST_EXIST = true;
        sparta::Register::Field* field =
            state->findRegister(reg_name, MUST_EXIST)->getField(field_name);
        // All field accesses are 64 bit
        const uint64_t value = getValueFromByteVector<uint64_t>(buffer);
        field->write(value);
    }

    void PegasusCoSim::pokeRegisterField(CoreId core_id, HartId hart_id, const std::string reg_name,
                                         const std::string field_name,
                                         std::vector<uint8_t> & buffer) const
    {
        auto state = pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id);
        constexpr bool MUST_EXIST = true;
        sparta::Register::Field* field =
            state->findRegister(reg_name, MUST_EXIST)->getField(field_name);
        // All field accesses are 64 bit
        const uint64_t value = getValueFromByteVector<uint64_t>(buffer);
        field->poke(value);
    }

    void PegasusCoSim::setPc(CoreId core_id, HartId hart_id, Addr addr)
    {
        // TODO: Create Event for PC override
        pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id)->setPc(addr);
    }

    Addr PegasusCoSim::getPc(CoreId core_id, HartId hart_id) const
    {
        return pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id)->getPc();
    }

    void PegasusCoSim::setPrivilegeMode(CoreId, HartId, PrivMode)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    PrivMode PegasusCoSim::getPrivilegeMode(CoreId, HartId) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    bool PegasusCoSim::isSimulationFinished(CoreId core_id, HartId hart_id) const
    {
        const PegasusState::SimState* sim_state =
            pegasus_sim_->getPegasusCore(core_id)->getPegasusState(hart_id)->getSimState();
        return sim_state->sim_stopped;
    }

    EventAccessor PegasusCoSim::injectInstruction(CoreId, HartId, Opcode)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    EventAccessor PegasusCoSim::injectInterrupt(CoreId, HartId, uint64_t)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    EventAccessor PegasusCoSim::injectReset(CoreId, HartId)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    uint64_t PegasusCoSim::getNumCommittedEvents(CoreId core_id, HartId hart_id) const
    {
        return pegasus_sim_->getPegasusCore(core_id)
                   ->getPegasusState(hart_id)
                   ->getSimState()
                   ->inst_count
               - getNumUncommittedEvents(core_id, hart_id);
    }

    EventAccessor PegasusCoSim::getLastCommittedEvent(CoreId core_id, HartId hart_id) const
    {
        auto evt_pipeline = getEventPipeline(core_id, hart_id);
        return evt_pipeline->getLastCommittedEvent();
    }

    const cosim::EventList & PegasusCoSim::getUncommittedEvents(CoreId core_id,
                                                                HartId hart_id) const
    {
        auto evt_pipeline = getEventPipeline(core_id, hart_id);
        return evt_pipeline->getUncommittedEvents();
    }

    uint64_t PegasusCoSim::getNumUncommittedEvents(CoreId core_id, HartId hart_id) const
    {
        return getUncommittedEvents(core_id, hart_id).size();
    }

    uint64_t PegasusCoSim::getNumUncommittedWrites(CoreId, HartId) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::readRegister_(sparta::Register* reg, std::vector<uint8_t> & buffer) const
    {
        const size_t size = reg->getNumBytes();
        buffer.resize(size);
        const size_t OFFSET = 0;
        reg->read(buffer.data(), size, OFFSET);
    }

    void PegasusCoSim::peekRegister_(sparta::Register* reg, std::vector<uint8_t> & buffer) const
    {
        const size_t size = reg->getNumBytes();
        buffer.resize(size);
        const size_t OFFSET = 0;
        reg->peek(buffer.data(), size, OFFSET);
    }

    void PegasusCoSim::writeRegister_(sparta::Register* reg, std::vector<uint8_t> & buffer) const
    {
        const size_t size = buffer.size();
        const size_t OFFSET = 0;
        reg->write(buffer.data(), size, OFFSET);
    }

    void PegasusCoSim::pokeRegister_(sparta::Register* reg, std::vector<uint8_t> & buffer) const
    {
        const size_t size = buffer.size();
        const size_t OFFSET = 0;
        reg->poke(buffer.data(), size, OFFSET);
    }

    void PegasusCoSim::onRegisterAppsRequest(simdb::AppRegistrations* app_registrations)
    {
        app_registrations->registerApp<CoSimEventPipeline>();
        app_registrations->registerApp<CoSimCheckpointer>();
    }

    void PegasusCoSim::onParameterizeAppsRequest(simdb::AppManager* app_mgr)
    {
        // Cache the AppManager
        app_mgr_ = app_mgr;

        // Before creating the apps, set up the ctor args that they need. Recall that the
        // checkpointer apps need to know the arch data root for each hart, which is where
        // the PegasusState lives for that hart. All instances however use the same scheduler.
        size_t pipeline_idx = 0;

        const size_t num_cores = cosim_observers_.size();
        for (CoreId core_idx = 0; core_idx < num_cores; ++core_idx)
        {
            for (HartId hart_idx = 0; hart_idx < num_harts_per_core_.at(core_idx); ++hart_idx)
            {
                auto state = pegasus_sim_->getPegasusCore(core_idx)->getPegasusState(hart_idx);
                auto system = pegasus_sim_->getPegasusCore(core_idx)->getSystem();

                std::vector<sparta::TreeNode*> chkptr_arch_data_roots;
                chkptr_arch_data_roots.push_back(state->getContainer());
                chkptr_arch_data_roots.push_back(system->getContainer());

                app_mgr->parameterizeAppFactoryInstance<CoSimEventPipeline>(pipeline_idx, core_idx,
                                                                            hart_idx, state);

                app_mgr->parameterizeAppFactoryInstance<CoSimCheckpointer>(
                    pipeline_idx, chkptr_arch_data_roots, scheduler_.get());

                ++pipeline_idx;
            }
        }
    }

    std::vector<std::string> PegasusCoSim::getWorkloadArgs_(const std::string & workload)
    {
        std::vector<std::string> workload_args;
        sparta::utils::tokenize_on_whitespace(workload, workload_args);
        return workload_args;
    }

    void PegasusCoSim::finish()
    {
        // Send remaining committed events down the pipeline(s) and shut down threads.
        pegasus_sim_->getAppManagers()->postSimLoopTeardown();

        std::cout << "Pegasus co-sim finished." << std::endl;
        for (CoreId core_id = 0; core_id < cosim_observers_.size(); ++core_id)
        {
            for (HartId hart_id = 0; hart_id < cosim_observers_.at(core_id).size(); ++hart_id)
            {
                auto evt_pipeline = getEventPipeline(core_id, hart_id);
                const auto euid = evt_pipeline->getLastEventUID();
                std::cout << "  Core " << core_id << ", Hart " << hart_id << ": ";
                std::cout << euid << " events processed." << std::endl;
            }
        }
    }
} // namespace pegasus::cosim
