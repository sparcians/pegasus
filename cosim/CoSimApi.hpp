#pragma once

#include "cosim/EventAccessor.hpp"
#include "cosim/MemoryInterface.hpp"

#include <deque>
#include <memory>

/**
 * \mainpage Pegasus CoSim API Proposal
 *
 * TODO
 *
 */
namespace pegasus::cosim
{
    using EventList = std::deque<Event>;

    /**
     * \class CoSim
     *
     * TODO
     *
     * Event
     *
     * Event List (Step, Commit and Flush)
     *
     * Step Actions
     *
     */
    class CoSim
    {
      public:
        //! Allow derivation
        virtual ~CoSim() {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Step, Commit, Flush

        /**
         * \brief Step the simulator at the current PC
         * \param hart The hart to step
         * \return The event object generated
         *
         * \note The event returned is a copy of what is appended to the event list.
         */
        virtual EventAccessor step(CoreId core_id, HartId hart_id) = 0;

        /**
         * \brief Step the simulator after overriding the current pc
         * \param hart The hart to step
         * \param override_pc Override value of pc
         * \return The event object generated
         *
         * \note The event returned is a copy of what is appended to the event list.
         */
        virtual EventAccessor step(CoreId core_id, HartId hart, Addr override_pc) = 0;

        /**
         * \brief Step the simulator to the next Action at the current pc
         * \param hart The hart to step
         * \return The event object generated
         *
         * \note The generated event may represent an incomplete step of
         *       the simulator and be only partially initialized.
         */
        virtual EventAccessor stepOperation(CoreId core_id, HartId hart_id) = 0;

        /**
         * \brief Step the simulator to the next Action after overriding the current pc
         * \param hart The hart to step
         * \param override_pc Override value of pc
         * \return The event object generated
         *
         * \note The generated event may represent an incomplete step of the simulator and be only
         *       partially initialized.
         */
        virtual EventAccessor stepOperation(CoreId core_id, HartId hart, Addr override_pc) = 0;

        /**
         * \brief Commit the oldest uncommitted Event in the event list
         * \param hart The hart to step
         */
        virtual void commit(CoreId core_id, HartId hart_id) = 0;

        /**
         * \brief Commit the event and all older events in the event list
         * \param event The event to commit
         */
        virtual void commit(cosim::EventAccessor & event) = 0;

        /**
         * \brief Commit the memory write(s) of a store instruction to memory
         * \param event The store event with memory write(s) to commit
         *
         * \throw If the event is not the oldest store event in the event list
         *
         * /note The store event must be the oldest store event in the event list with uncommitted
         *       memory write(s). Store events must commit their memory writes in order relative to
         *       other store events. Store events can commit their memory writes out of order using
         *       the comitStoreWrite(event, paddr) method or drop their memory writes using the
         *       dropStoreWrite(event) and dropStoreWrite(event, paddr) methods.
         */
        virtual void commitStoreWrite(cosim::EventAccessor & event) = 0;

        /**
         * \brief Commit a specified memory write of a store instruction
         * \param event The store event with memory write(s) to commit
         * \param paddr The physical address of the memory write to commit
         *
         * \throw If the event is not the oldest store event in the event list
         *
         * /note The store event must be the oldest store event in the event list with uncommitted
         *       memory write(s). Store events must commit their memory writes in order relative to
         *       other store events. Store events can commit their memory writes out of order using
         *       the comitStoreWrite(event, paddr) method or drop their memory writes using the
         *       dropStoreWrite(event) and dropStoreWrite(event, paddr) methods.
         */
        virtual void commitStoreWrite(cosim::EventAccessor & event, Addr paddr) = 0;

        /**
         * \brief Drop a store event's memory write(s)
         * \param event The store event with memory write(s) to drop
         */
        virtual void dropStoreWrite(cosim::EventAccessor & event) = 0;

        /**
         * \brief Drop a store event's memory write to the specified physical address
         * \param event The store event with memory write(s) to drop
         * \param paddr The physical address of the memory write to drop
         */
        virtual void dropStoreWrite(cosim::EventAccessor & event, Addr paddr) = 0;

        /**
         * \brief Flush the event and all younger uncommitted events
         * \param event The event to flush
         * \param flush_younger_only If true, only flush younger uncommitted events
         *
         * \note Flushing an event reverts its changes to the program state, drops its memory
         *       write(s) and removes it from the event list. Events will be flushed from youngest
         *       to oldest.
         */
        virtual void flush(cosim::EventAccessor & event, bool flush_younger_only = false) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Memory Interface

        /**
         * \brief Get the pointer to the cosim's memory interface
         * \return Pointer to the cosim's memory interface
         */
        virtual cosim::MemoryInterface* getMemoryInterface() = 0;

        /**
         * \brief Set the cosim's memory interface
         * \param mem_if Pointer to a cosim::MemoryInterface object
         */
        virtual void setMemoryInterface(cosim::MemoryInterface* mem_if) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Program State

        virtual void readRegister(CoreId core_id, HartId hart, RegId reg,
                                  std::vector<uint8_t> & buffer) const = 0;
        virtual void peekRegister(CoreId core_id, HartId hart, RegId reg,
                                  std::vector<uint8_t> & buffer) const = 0;
        virtual void writeRegister(CoreId core_id, HartId hart, RegId reg,
                                   std::vector<uint8_t> & buffer) const = 0;
        virtual void pokeRegister(CoreId core_id, HartId hart, RegId reg,
                                  std::vector<uint8_t> & buffer) const = 0;

        virtual void readRegister(CoreId core_id, HartId hart, const std::string reg_name,
                                  std::vector<uint8_t> & buffer) const = 0;
        virtual void peekRegister(CoreId core_id, HartId hart, const std::string reg_name,
                                  std::vector<uint8_t> & buffer) const = 0;
        virtual void writeRegister(CoreId core_id, HartId hart, const std::string reg_name,
                                   std::vector<uint8_t> & buffer) const = 0;
        virtual void pokeRegister(CoreId core_id, HartId hart, const std::string reg_name,
                                  std::vector<uint8_t> & buffer) const = 0;

        virtual void readRegisterField(CoreId core_id, HartId hart, const std::string reg_name,
                                       const std::string field_name,
                                       std::vector<uint8_t> & buffer) const = 0;
        virtual void peekRegisterField(CoreId core_id, HartId hart, const std::string reg_name,
                                       const std::string field_name,
                                       std::vector<uint8_t> & buffer) const = 0;
        virtual void writeRegisterField(CoreId core_id, HartId hart, const std::string reg_name,
                                        const std::string field_name,
                                        std::vector<uint8_t> & buffer) const = 0;
        virtual void pokeRegisterField(CoreId core_id, HartId hart, const std::string reg_name,
                                       const std::string field_name,
                                       std::vector<uint8_t> & buffer) const = 0;

        virtual void setPc(CoreId core_id, HartId hart, Addr pc) = 0;
        virtual Addr getPc(CoreId core_id, HartId hart_id) const = 0;

        virtual void setPrivilegeMode(CoreId core_id, HartId hart, PrivMode priv_mode) = 0;
        virtual PrivMode getPrivilegeMode(CoreId core_id, HartId hart_id) const = 0;

        virtual bool isSimulationFinished(CoreId core_id, HartId hart_id) const = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Translation (TODO: need to determine suitable return type)

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Injection

        virtual EventAccessor injectInstruction(CoreId core_id, HartId hart, Opcode opcode) = 0;
        virtual EventAccessor injectInterrupt(CoreId core_id, HartId hart,
                                              uint64_t interrupt_code) = 0;
        virtual EventAccessor injectReset(CoreId core_id, HartId hart_id) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Checkpointing (TODO: is this necessary?)

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Debug

        virtual uint64_t getNumCommittedEvents(CoreId core_id, HartId hart_id) const = 0;
        virtual EventAccessor getLastCommittedEvent(CoreId core_id, HartId hart_id) const = 0;
        virtual const cosim::EventList & getUncommittedEvents(CoreId core_id,
                                                              HartId hart_id) const = 0;
        virtual uint64_t getNumUncommittedEvents(CoreId core_id, HartId hart_id) const = 0;
        virtual uint64_t getNumUncommittedWrites(CoreId core_id, HartId hart_id) const = 0;
    };

    /**
     * \brief Create an instance of the CoSim interface
     * \param config_yaml YAML-based configuration file
     * \return A pointer to the CoSim instance
     *
     * \todo Need to define extension for cosim params so we can sue yamls as config files
     */
    std::unique_ptr<CoSim> createCoSimInstance(const std::string & config_yaml);
} // namespace pegasus::cosim
