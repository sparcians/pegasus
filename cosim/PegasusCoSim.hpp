#pragma once

#include <cinttypes>
#include <string>
#include <vector>
#include <map>

#include "cosim/CoSimApi.hpp"
#include "core/SimListener.hpp"

namespace pegasus
{
    class PegasusSim;
    class Fetch;
} // namespace pegasus

namespace sparta
{
    class Scheduler;

    namespace app
    {
        class SimulationConfiguration;
    }

    namespace log
    {
        class MessageSource;
        class Tap;
    } // namespace log

    namespace memory
    {
        class SimpleMemoryMapNode;
    }

    class Register;
} // namespace sparta

namespace simdb
{
    class AppManager;
    class DatabaseManager;
} // namespace simdb

namespace pegasus::cosim
{
    class CoSimObserver;

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
    class CoSimEventPipeline;

    class PegasusCoSim : public pegasus::cosim::CoSim, private pegasus::SimListener
    {
      public:
        PegasusCoSim(uint64_t ilimit = 0, const std::string & workload = "",
                     const std::map<std::string, std::string> pegasus_params = {},
                     const std::string & db_file = "pegasus-cosim.db",
                     const size_t snapshot_threshold = 100);

        ~PegasusCoSim() noexcept;

        void enableLogger(const std::string & filename = "");
        void logMessage(const std::string & message);

        CoSimEventPipeline* getEventPipeline(CoreId core_id, HartId hart_id);
        const CoSimEventPipeline* getEventPipeline(CoreId core_id, HartId hart_id) const;

        EventAccessor step(CoreId core_id, HartId hart_id) override final;
        EventAccessor step(CoreId core_id, HartId hart_id, Addr override_pc) override final;

        // Unimplemented methods
        EventAccessor stepOperation(CoreId core_id, HartId hart_id) override final;
        EventAccessor stepOperation(CoreId core_id, HartId hart_id,
                                    Addr override_pc) override final;
        void commit(CoreId core_id, HartId hart_id) override final;
        void commit(cosim::EventAccessor & event) override final;
        void commitStoreWrite(cosim::EventAccessor & event) override final;
        void commitStoreWrite(cosim::EventAccessor & event, Addr paddr) override final;
        void dropStoreWrite(cosim::EventAccessor & event) override final;
        void dropStoreWrite(cosim::EventAccessor & event, Addr paddr) override final;
        void flush(cosim::EventAccessor & event, bool flush_younger_only = false) override final;
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
        void readRegister(CoreId core_id, HartId hart_id, const std::string reg_name,
                          std::vector<uint8_t> & buffer) const override final;
        void peekRegister(CoreId core_id, HartId hart_id, const std::string reg_name,
                          std::vector<uint8_t> & buffer) const override final;
        void writeRegister(CoreId core_id, HartId hart_id, const std::string reg_name,
                           std::vector<uint8_t> & buffer) const override final;
        void pokeRegister(CoreId core_id, HartId hart_id, const std::string reg_name,
                          std::vector<uint8_t> & buffer) const override final;
        void readRegisterField(CoreId core_id, HartId hart_id, const std::string reg_name,
                               const std::string field_name,
                               std::vector<uint8_t> & buffer) const override final;
        void peekRegisterField(CoreId core_id, HartId hart_id, const std::string reg_name,
                               const std::string field_name,
                               std::vector<uint8_t> & buffer) const override final;
        void writeRegisterField(CoreId core_id, HartId hart_id, const std::string reg_name,
                                const std::string field_name,
                                std::vector<uint8_t> & buffer) const override final;
        void pokeRegisterField(CoreId core_id, HartId hart_id, const std::string reg_name,
                               const std::string field_name,
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
        EventAccessor getLastCommittedEvent(CoreId core_id, HartId hart_id) const override final;
        const cosim::EventList & getUncommittedEvents(CoreId core_id,
                                                      HartId hart_id) const override final;
        uint64_t getNumUncommittedEvents(CoreId core_id, HartId hart_id) const override final;
        uint64_t getNumUncommittedWrites(CoreId core_id, HartId hart_id) const override final;

        void finish();

        // Debug/testing
        const pegasus::PegasusSim & getPegasusSim() const { return *pegasus_sim_.get(); }

      private:
        void readRegister_(sparta::Register* reg, std::vector<uint8_t> & buffer) const;
        void peekRegister_(sparta::Register* reg, std::vector<uint8_t> & buffer) const;
        void writeRegister_(sparta::Register* reg, std::vector<uint8_t> & buffer) const;
        void pokeRegister_(sparta::Register* reg, std::vector<uint8_t> & buffer) const;
        void onRegisterAppsRequest(simdb::AppRegistrations* app_registrations) override;
        void onParameterizeAppsRequest(simdb::AppManager* app_mgr) override;

        static std::vector<std::string> getWorkloadArgs_(const std::string & workload);

        // CoSim Logger
        std::unique_ptr<sparta::log::MessageSource> cosim_logger_;
        std::unique_ptr<sparta::log::Tap> sparta_tap_;

        // Sparta components
        std::unique_ptr<sparta::Scheduler> scheduler_;

        // The main simulator
        std::unique_ptr<pegasus::PegasusSim> pegasus_sim_;

        // Number of harts per core (indexed by core)
        std::vector<uint32_t> num_harts_per_core_;

        // Handy list of fetching blocks
        std::vector<std::vector<Fetch*>> fetch_;

        // CoSim memory interface
        CoSimMemoryInterface* cosim_memory_if_ = nullptr;

        // Sim config for sparta::app::Simulation base class
        std::unique_ptr<sparta::app::SimulationConfiguration> sim_config_;

        // SimDB app manager to manage the CoSimEventPipeline and CoSimCheckpointer apps
        simdb::AppManager* app_mgr_ = nullptr;

        // Cached cosim observers for each hart
        std::vector<std::vector<CoSimObserver*>> cosim_observers_;
    };
} // namespace pegasus::cosim
