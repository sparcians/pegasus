#include "cosim/PegasusCoSim.hpp"
#include "core/Fetch.hpp"
#include "core/observers/CoSimObserver.hpp"
#include "include/ActionTags.hpp"

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/SpartaAssert.hpp"

#include "cosim/CoSimPipeline.hpp"
#include "simdb/sqlite/DatabaseManager.hpp"
#include "simdb/apps/AppManager.hpp"

namespace pegasus::cosim
{
    CoSimMemoryInterface::CoSimMemoryInterface(sparta::memory::SimpleMemoryMapNode* memory) :
        memory_(memory)
    {
    }

    bool CoSimMemoryInterface::peek(HartId, Addr paddr, size_t size,
                                    std::vector<uint8_t> & buffer) const
    {
        const bool success = memory_->tryPeek(paddr, size, buffer.data());
        return success;
    }

    bool CoSimMemoryInterface::read(HartId, Addr paddr, size_t size,
                                    std::vector<uint8_t> & buffer) const
    {
        const bool success = memory_->tryRead(paddr, size, buffer.data());
        return success;
    }

    bool CoSimMemoryInterface::poke(HartId, Addr paddr, std::vector<uint8_t> & buffer) const
    {
        const size_t size = buffer.size() * sizeof(uint8_t);
        const bool success = memory_->tryPoke(paddr, size, buffer.data());
        return success;
    }

    bool CoSimMemoryInterface::write(HartId, Addr paddr, std::vector<uint8_t> & buffer) const
    {
        const size_t size = buffer.size() * sizeof(uint8_t);
        const bool success = memory_->tryWrite(paddr, size, buffer.data());
        return success;
    }

    PegasusCoSim::PegasusCoSim(sparta::Scheduler* scheduler, uint64_t ilimit,
                               const std::string & workload) :
        PegasusSim(scheduler, getWorkloadArgs_(workload), {}, ilimit),
        cosim_logger_(getRoot(), "cosim", "Pegasus Cosim Logger")
    {
        configure(0, nullptr, &sim_config_);
        buildTree();
        configureTree();
        finalizeTree();
        finalizeFramework();

        const uint32_t num_harts = 1;

        event_list_.resize(num_harts);
        last_committed_event_.resize(num_harts, cosim::Event(cosim::Event::Type::INVALID));

        db_mgr_ = std::make_shared<simdb::DatabaseManager>("pegasus-cosim.db", true);
        app_mgr_ = std::make_shared<simdb::AppManager>(db_mgr_.get());

        app_mgr_->enableApp(CoSimPipeline::NAME);
        app_mgr_->createEnabledApps();
        app_mgr_->createSchemas();
        app_mgr_->postInit(0, nullptr);

        cosim_pipeline_ = app_mgr_->getApp<CoSimPipeline>();
        cosim_pipeline_->setNumHarts(num_harts);
        app_mgr_->openPipelines();

        for (uint32_t hart_id = 0; hart_id < num_harts; ++hart_id)
        {
            // Get Fetch for each hart
            const std::string core_name = "core" + std::to_string(hart_id);
            fetch_.emplace_back(
                getRoot()->getChild(core_name + ".fetch")->getResourceAs<pegasus::Fetch>());

            auto state = getPegasusState(hart_id);

            // Create and attach CoSimObserver to PegasusState for each hart
            auto cosim_obs =
                std::make_unique<CoSimObserver>(cosim_logger_, cosim_pipeline_, hart_id);
            cosim_observers_.emplace_back(cosim_obs.get());
            state->addObserver(std::move(cosim_obs));
        }

        // Single memory IF for all harts
        cosim_memory_if_ = new CoSimMemoryInterface(getPegasusSystem()->getSystemMemory());
    }

    PegasusCoSim::~PegasusCoSim() noexcept { getRoot()->enterTeardown(); }

    void PegasusCoSim::enableLogger(const std::string & filename)
    {
        if (!filename.empty())
        {
            sparta_tap_.reset(new sparta::log::Tap(getRoot(), "cosim", filename));
        }
        else
        {
            sparta_tap_.reset(new sparta::log::Tap(getRoot(), "cosim", std::cout));
        }
    }

    EventAccessor PegasusCoSim::step(HartId hart_id)
    {
        ActionGroup* next_action_group = fetch_.at(hart_id)->getActionGroup();
        do
        {
            next_action_group = next_action_group->execute(getPegasusState(hart_id));
        } while (next_action_group && (next_action_group->hasTag(ActionTags::FETCH_TAG) == false));

        return cosim_pipeline_->getLastEvent(hart_id);

#if 0
        event_list_.at(hart_id).emplace_back(event);
        return event_list_.at(hart_id).back();
#endif
    }

    EventAccessor PegasusCoSim::step(HartId hart_id, Addr addr)
    {
        setPc(hart_id, addr);
        return step(hart_id);
    }

    EventAccessor PegasusCoSim::stepOperation(HartId)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    EventAccessor PegasusCoSim::stepOperation(HartId, Addr)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::commit(HartId hart_id)
    {
#if 0
        const cosim::Event & event = event_list_.at(hart_id).front();
        // COSIMLOG(event);
        sparta_assert(event.isDone(), "Cannot commit an event that isn't done! " << event);

        last_committed_event_.at(hart_id) = event;
        event_list_.at(hart_id).pop_front();
        // TODO: commit store writes
#endif
        (void)hart_id;
    }

    void PegasusCoSim::commit(const cosim::Event* event)
    {
#if 0
        const HartId hart_id = event->getHartId();

        // Find event in the event list
        const cosim::EventList & event_list = event_list_.at(hart_id);
        auto it = std::find_if(event_list.begin(), event_list.end(), [event](const cosim::Event & e)
                               { return e.getEuid() == event->getEuid(); });
        sparta_assert(it != event_list.end(), "Event not found in event list!");

        // Commit all events up to and including the event
        const uint32_t num_events_to_commit = std::distance(event_list.begin(), std::next(it));
        for (uint32_t i = 0; i < num_events_to_commit; ++i)
        {
            commit(hart_id);
        }
        COSIMLOG("Number of events committed: " << num_events_to_commit);
#endif
        (void)event;
    }

    void PegasusCoSim::commitStoreWrite(const cosim::Event*)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::commitStoreWrite(const cosim::Event*, Addr)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::dropStoreWrite(const cosim::Event*)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::dropStoreWrite(const cosim::Event*, Addr)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::flush(const cosim::Event*, bool)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    cosim::MemoryInterface* PegasusCoSim::getMemoryInterface() { return cosim_memory_if_; }

    void PegasusCoSim::setMemoryInterface(cosim::MemoryInterface*)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::readRegister(HartId, RegId, std::vector<uint8_t> &) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::peekRegister(HartId, RegId, std::vector<uint8_t> &) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::writeRegister(HartId, RegId, std::vector<uint8_t> &) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::pokeRegister(HartId, RegId, std::vector<uint8_t> &) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    void PegasusCoSim::setPc(HartId hart_id, Addr addr)
    {
        // TODO: Create Event for PC override
        getPegasusState(hart_id)->setPc(addr);
    }

    Addr PegasusCoSim::getPc(HartId hart_id) const { return getPegasusState(hart_id)->getPc(); }

    void PegasusCoSim::setPrivilegeMode(HartId, PrivMode)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    PrivMode PegasusCoSim::getPrivilegeMode(HartId) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    bool PegasusCoSim::isSimulationFinished(HartId hart_id) const
    {
        const PegasusState::SimState* sim_state = getPegasusState(hart_id)->getSimState();
        return sim_state->sim_stopped;
    }

    EventAccessor PegasusCoSim::injectInstruction(HartId, Opcode)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    EventAccessor PegasusCoSim::injectInterrupt(HartId, uint64_t)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    EventAccessor PegasusCoSim::injectReset(HartId)
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    uint64_t PegasusCoSim::getNumCommittedEvents(HartId hart_id) const
    {
        return getPegasusState(hart_id)->getSimState()->inst_count
               - getNumUncommittedEvents(hart_id);
    }

    const cosim::Event & PegasusCoSim::getLastCommittedEvent(HartId hart_id) const
    {
        return last_committed_event_.at(hart_id);
    }

    const cosim::EventList & PegasusCoSim::getUncommittedEvents(HartId hart_id) const
    {
        return event_list_.at(hart_id);
    }

    uint64_t PegasusCoSim::getNumUncommittedEvents(HartId hart_id) const
    {
        return event_list_.at(hart_id).size();
    }

    uint64_t PegasusCoSim::getNumUncommittedWrites(HartId) const
    {
        sparta_assert(false, "CoSim method is not implemented!");
    }

    std::vector<std::string> PegasusCoSim::getWorkloadArgs_(const std::string & workload)
    {
        std::vector<std::string> workload_args;
        sparta::utils::tokenize_on_whitespace(workload, workload_args);
        return workload_args;
    }

    void PegasusCoSim::finish()
    {
        const auto euid = cosim_pipeline_->getLastEventUID();
        std::cout << "Pegasus co-sim finished after processing " << euid << " events. "
                  << std::endl;
        app_mgr_->postSimLoopTeardown();
    }
} // namespace pegasus::cosim
