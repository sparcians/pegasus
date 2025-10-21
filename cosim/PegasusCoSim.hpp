#pragma once

#include "sim/PegasusSim.hpp"
#include "cosim/CoSimApi.hpp"
#include "sparta/app/SimulationConfiguration.hpp"

namespace pegasus
{
    class Fetch;
}

namespace simdb
{
    class AppManager;
    class DatabaseManager;
} // namespace simdb

namespace pegasus::cosim
{
    class CoSimMemoryInterface : public cosim::MemoryInterface
    {
      public:
        CoSimMemoryInterface(sparta::memory::SimpleMemoryMapNode* memory_map);

        ~CoSimMemoryInterface() {}

        bool peek(CoreId core_id, HartId hart_id, Addr paddr, size_t size,
                  std::vector<uint8_t> & buffer) const override;
        bool read(CoreId core_id, HartId hart_id, Addr paddr, size_t size,
                  std::vector<uint8_t> & buffer) const override;
        bool poke(CoreId core_id, HartId hart_id, Addr paddr,
                  std::vector<uint8_t> & buffer) const override;
        bool write(CoreId core_id, HartId hart_id, Addr paddr,
                   std::vector<uint8_t> & buffer) const override;

      private:
        sparta::memory::SimpleMemoryMapNode* memory_ = nullptr;
    };

    class CoSimObserver;
    class CoSimPipeline;

    class PegasusCoSim : public PegasusSim, public pegasus::cosim::CoSim
    {
      public:
        PegasusCoSim(sparta::Scheduler* scheduler, uint64_t ilimit = 0,
                     const std::string & workload = "",
                     const uint32_t event_window_size = DEFAULT_EVENT_WINDOW_SIZE);

        ~PegasusCoSim() noexcept;

        void enableLogger(const std::string & filename = "");

        CoSimPipeline* getCoSimPipeline() const { return cosim_pipeline_; }

        EventAccessor step(CoreId core_id, HartId hart_id) override final;
        EventAccessor step(CoreId core_id, HartId hart_id, Addr override_pc) override final;

        // Unimplemented methods
        EventAccessor stepOperation(CoreId core_id, HartId hart_id) override final;
        EventAccessor stepOperation(CoreId core_id, HartId hart_id,
                                    Addr override_pc) override final;
        void commit(CoreId core_id, HartId hart_id) override final;
        void commit(const cosim::Event* event) override final;
        void commitStoreWrite(const cosim::Event* event) override final;
        void commitStoreWrite(const cosim::Event* event, Addr paddr) override final;
        void dropStoreWrite(const cosim::Event* event) override final;
        void dropStoreWrite(const cosim::Event* event, Addr paddr) override final;
        void flush(const cosim::Event* event, bool flush_younger_only = false) override final;
        cosim::MemoryInterface* getMemoryInterface() override final;
        void setMemoryInterface(cosim::MemoryInterface* mem_if) override final;
        void readRegister(CoreId core_id, HartId hart_id, RegId reg,
                          std::vector<uint8_t> & buffer) const override final;
        void peekRegister(CoreId core_id, HartId hart_id, RegId reg,
                          std::vector<uint8_t> & buffer) const override final;
        void writeRegister(CoreId core_id, HartId hart_id, RegId reg,
                           std::vector<uint8_t> & buffer) const override final;
        void pokeRegister(CoreId core_id, HartId hart_id, RegId reg,
                          std::vector<uint8_t> & buffer) const override final;
        void setPc(CoreId core_id, HartId hart_id, Addr pc) override final;
        Addr getPc(CoreId core_id, HartId hart_id) const override final;
        void setPrivilegeMode(CoreId core_id, HartId hart_id, PrivMode priv_mode) override final;
        PrivMode getPrivilegeMode(CoreId core_id, HartId hart_id) const override final;
        bool isSimulationFinished(CoreId core_id, HartId hart_id) const override final;
        EventAccessor injectInstruction(CoreId core_id, HartId hart_id,
                                        Opcode opcode) override final;
        EventAccessor injectInterrupt(CoreId core_id, HartId hart_id,
                                      uint64_t interrupt_code) override final;
        EventAccessor injectReset(CoreId core_id, HartId hart_id) override final;
        uint64_t getNumCommittedEvents(CoreId core_id, HartId hart_id) const override final;
        const cosim::Event & getLastCommittedEvent(CoreId core_id,
                                                   HartId hart_id) const override final;
        const cosim::EventList & getUncommittedEvents(CoreId core_id,
                                                      HartId hart_id) const override final;
        uint64_t getNumUncommittedEvents(CoreId core_id, HartId hart_id) const override final;
        uint64_t getNumUncommittedWrites(CoreId core_id, HartId hart_id) const override final;

        void finish();

      private:
        static std::vector<std::string> getWorkloadArgs_(const std::string & workload);

        // CoSim Logger
        sparta::log::MessageSource cosim_logger_;
        std::unique_ptr<sparta::log::Tap> sparta_tap_;

        // Fetch Unit for each hart
        std::vector<std::vector<Fetch*>> fetch_;

        // CoSim memory interface
        CoSimMemoryInterface* cosim_memory_if_ = nullptr;

        // Event List for each hart
        std::vector<std::vector<EventList>> event_list_;

        // Last committed event for each hart
        std::vector<std::vector<Event>> last_committed_event_;

        // Sim config for sparta::app::Simulation base class
        sparta::app::SimulationConfiguration sim_config_;

        // CoSim pipeline for retrieving events and checkpoints
        CoSimPipeline* cosim_pipeline_ = nullptr;

        // SimDB instance to hold all events and checkpoints
        std::shared_ptr<simdb::DatabaseManager> db_mgr_;

        // SimDB app manager to manage the CoSimPipeline
        std::shared_ptr<simdb::AppManager> app_mgr_;

        // CoSim Observer for capturing Events from each hart
        std::vector<CoSimObserver*> cosim_observers_;
    };
} // namespace pegasus::cosim
