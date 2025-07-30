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

        bool peek(HartId hart, Addr paddr, size_t size,
                  std::vector<uint8_t> & buffer) const override;
        bool read(HartId hart, Addr paddr, size_t size,
                  std::vector<uint8_t> & buffer) const override;
        bool poke(HartId hart, Addr paddr, std::vector<uint8_t> & buffer) const override;
        bool write(HartId hart, Addr paddr, std::vector<uint8_t> & buffer) const override;

      private:
        sparta::memory::SimpleMemoryMapNode* memory_ = nullptr;
    };

    class CoSimObserver;
    class CoSimPipeline;

    class PegasusCoSim : public PegasusSim, public pegasus::cosim::CoSim
    {
      public:
        PegasusCoSim(sparta::Scheduler* scheduler, uint64_t ilimit = 0,
                     const std::string & workload = "");

        ~PegasusCoSim() noexcept;

        void enableLogger(const std::string & filename = "");

        CoSimPipeline* getCoSimPipeline() const { return cosim_pipeline_; }

        EventAccessor step(HartId hart) override final;
        EventAccessor step(HartId hart, Addr override_pc) override final;

        // Unimplemented methods
        EventAccessor stepOperation(HartId hart) override final;
        EventAccessor stepOperation(HartId hart, Addr override_pc) override final;
        void commit(HartId hart) override final;
        void commit(const cosim::Event* event) override final;
        void commitStoreWrite(const cosim::Event* event) override final;
        void commitStoreWrite(const cosim::Event* event, Addr paddr) override final;
        void dropStoreWrite(const cosim::Event* event) override final;
        void dropStoreWrite(const cosim::Event* event, Addr paddr) override final;
        void flush(const cosim::Event* event, bool flush_younger_only = false) override final;
        cosim::MemoryInterface* getMemoryInterface() override final;
        void setMemoryInterface(cosim::MemoryInterface* mem_if) override final;
        void readRegister(HartId hart, RegId reg,
                          std::vector<uint8_t> & buffer) const override final;
        void peekRegister(HartId hart, RegId reg,
                          std::vector<uint8_t> & buffer) const override final;
        void writeRegister(HartId hart, RegId reg,
                           std::vector<uint8_t> & buffer) const override final;
        void pokeRegister(HartId hart, RegId reg,
                          std::vector<uint8_t> & buffer) const override final;
        void setPc(HartId hart, Addr pc) override final;
        Addr getPc(HartId hart) const override final;
        void setPrivilegeMode(HartId hart, PrivMode priv_mode) override final;
        PrivMode getPrivilegeMode(HartId hart) const override final;
        bool isSimulationFinished(HartId hart) const override final;
        EventAccessor injectInstruction(HartId hart, Opcode opcode) override final;
        EventAccessor injectInterrupt(HartId hart, uint64_t interrupt_code) override final;
        EventAccessor injectReset(HartId hart) override final;
        uint64_t getNumCommittedEvents(HartId hart) const override final;
        const cosim::Event & getLastCommittedEvent(HartId hart) const override final;
        const cosim::EventList & getUncommittedEvents(HartId hart) const override final;
        uint64_t getNumUncommittedEvents(HartId hart) const override final;
        uint64_t getNumUncommittedWrites(HartId hart) const override final;

        void finish();

      private:
        static std::vector<std::string> getWorkloadArgs_(const std::string & workload);

        // CoSim Logger
        sparta::log::MessageSource cosim_logger_;
        std::unique_ptr<sparta::log::Tap> sparta_tap_;

        // Fetch Unit for each hart
        std::vector<Fetch*> fetch_;

        // CoSim memory interface
        CoSimMemoryInterface* cosim_memory_if_ = nullptr;

        // Event List for each hart
        std::vector<EventList> event_list_;

        // Last committed event for each hart
        std::vector<Event> last_committed_event_;

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
