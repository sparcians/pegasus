#pragma once

#include "include/PegasusTypes.hpp"
#include "include/PegasusUtils.hpp"
#include "mavis/OpcodeInfo.h"
#include "sparta/utils/ValidValue.hpp"

#include <vector>
#include <algorithm>
#include <iomanip>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

namespace pegasus
{
    class PegasusState;
}

namespace pegasus::cosim
{
    class CoSimEventPipeline;
    class CoSimObserver;

    /*!
     * \class Event
     *
     * \brief Represents a change to the program state that occurs durring cosimulation.
     *
     * The Event class encapsulates various types of events that can occur during cosimulation like
     * instruction execution, interrupts, injected behaviors and register/memory writes.
     *
     * The Event class supports copy construction and assignment, and it provides friend access
     * to the CoSimObserver class for detailed observation of events.
     */
    class Event
    {
      public:
        enum class Type
        {
            INSTRUCTION,
            INTERRUPT,
            INJECTED_INSTRUCTION,
            INJECTED_INTERRUPT,
            REGISTER_WRITE,
            MEMORY_WRITE,
            INVALID
        };

        struct RegReadAccess
        {
            RegId reg_id;
            std::vector<uint8_t> value;

            RegReadAccess(RegId id, const std::vector<uint8_t> & val) : reg_id(id), value(val) {}

            RegReadAccess(RegId id, const uint64_t val) :
                reg_id(id),
                value(convertToByteVector(val))
            {
            }

            RegReadAccess() = default;

            /// Called to/from char buffer (boost::serialization)
            template <typename Archive> void serialize(Archive & ar, const unsigned int /*version*/)
            {
                ar & reg_id;
                ar & value;
            }

            bool operator==(const RegReadAccess & other) const = default;
        };

        struct RegWriteAccess : public RegReadAccess
        {
            std::vector<uint8_t> prev_value;

            RegWriteAccess(RegId id, const std::vector<uint8_t> & val,
                           const std::vector<uint8_t> & prev_val) :
                RegReadAccess(id, val),
                prev_value(prev_val)
            {
            }

            RegWriteAccess(RegId id, const uint64_t val, const uint64_t prev_val) :
                RegReadAccess(id, val),
                prev_value(convertToByteVector(prev_val))
            {
            }

            RegWriteAccess() = default;

            /// Called to/from char buffer (boost::serialization)
            template <typename Archive> void serialize(Archive & ar, const unsigned int version)
            {
                RegReadAccess::serialize(ar, version);
                ar & prev_value;
            }

            bool operator==(const RegWriteAccess & other) const = default;
        };

        struct MemReadAccess
        {
            MemAccessSource source;
            Addr paddr;
            Addr vaddr;
            size_t size;
            std::vector<uint8_t> value;

            /// Called to/from char buffer (boost::serialization)
            template <typename Archive> void serialize(Archive & ar, const unsigned int /*version*/)
            {
                ar & source;
                ar & paddr;
                ar & vaddr;
                ar & size;
                ar & value;
            }

            bool operator==(const MemReadAccess & other) const = default;
        };

        struct MemWriteAccess : public MemReadAccess
        {
            std::vector<uint8_t> prev_value;

            /// Called to/from char buffer (boost::serialization)
            template <typename Archive> void serialize(Archive & ar, const unsigned int version)
            {
                MemReadAccess::serialize(ar, version);
                ar & prev_value;
            }

            bool operator==(const MemWriteAccess & other) const = default;
        };

        static inline constexpr auto INVALID_EVENT_UID = std::numeric_limits<uint64_t>::max();

        Event() = default;

        Event(Type type) : type_(type) {}

        uint64_t getEuid() const { return event_uid_; }

        uint64_t getSimStateCurrentUID() const { return sim_state_current_uid_; }

        Type getEventType() const { return type_; }

        CoreId getCoreId() const { return core_id_; }

        HartId getHartId() const { return hart_id_; }

        bool isDone() const { return done_; }

        bool isLastEvent() const { return event_ends_sim_; }

        bool isEventInRoi() const { return is_in_region_of_interest_; }

        bool isEventEnteringRoi() const { return is_entering_region_of_interest_; }

        bool isEventExitingRoi() const { return is_exiting_region_of_interest_; }

        uint64_t getArchId() const { return arch_id_; }

        Opcode getOpcode() const { return opcode_; }

        OpcodeSize getOpcodeSize() const { return opcode_size_; }

        InstType getInstType() const { return inst_type_; }

        bool isChangeOfFlowEvent() const { return is_change_of_flow_; }

        Addr getPc() const { return curr_pc_; }

        Addr getNextPc() const { return next_pc_; }

        Addr getAltNextPc() const { return alternate_next_pc_; }

        PrivMode getPrivilegeMode() const { return curr_priv_; }

        PrivMode getNextPrivilegeMode() const { return next_priv_; }

        PrivMode getLdStPrivilegeMode() const { return curr_ldst_priv_; }

        PrivMode getNextLdStPrivilegeMode() const { return next_ldst_priv_; }

        ExcpType getExceptionType() const { return excp_type_; }

        ExcpCode getExceptionCode() const { return excp_code_; }

        ExcpCode getPrevExceptionCode() const { return prev_excp_code_; }

        bool hasCsr() const { return inst_csr_ != std::numeric_limits<uint32_t>::max(); }

        uint32_t getCsr() const
        {
            sparta_assert(hasCsr(), "Event does not have a valid CSR!");
            return inst_csr_;
        }

        const std::vector<RegReadAccess> & getRegisterReads() const { return register_reads_; }

        const std::vector<RegWriteAccess> & getRegisterWrites() const { return register_writes_; }

        const std::vector<MemReadAccess> & getMemoryReads() const { return memory_reads_; }

        const std::vector<MemWriteAccess> & getMemoryWrites() const { return memory_writes_; }

        const std::string & getDisassemblyStr() const { return dasm_string_; }

        const sparta::utils::ValidValue<Addr> & getStartReservation() const
        {
            return start_reservation_;
        }

        const sparta::utils::ValidValue<Addr> & getEndReservation() const
        {
            return end_reservation_;
        }

        bool operator==(const Event & other) const = default;

      private:
        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \name
        //! @{

        // Event info
        sparta::utils::ValidValue<uint64_t> event_uid_; //!< Unique ID of Event
        uint64_t sim_state_current_uid_ = 0;            //!< Current UID in PegasusState::SimState
        Type type_ = Type::INVALID;                     //!< Type of Event
        CoreId core_id_ = std::numeric_limits<CoreId>::max(); //!< Core ID of Event
        HartId hart_id_ = std::numeric_limits<HartId>::max(); //!< Hart ID of Event
        bool done_{false};                                    //!< Is the Event finished executing?
        bool event_ends_sim_{false}; //!< Will committing this Event end simulation?

        // Region of interest
        bool is_in_region_of_interest_{
            true}; //!< True if this Event occurred in the region of interest
        bool is_entering_region_of_interest_{
            false}; //!< True if the next Event will be in the region of interest
        bool is_exiting_region_of_interest_{
            false}; //!< True if the next Event will not be in the region of interest

        // Instruction info
        uint64_t arch_id_ = std::numeric_limits<uint64_t>::max(); //!< Architectural ID of Event
        Opcode opcode_ = std::numeric_limits<Opcode>::max();      //!< Opcode for instruction Events
        OpcodeSize opcode_size_ =
            std::numeric_limits<OpcodeSize>::max(); //!< Opcode size for instruction Events

        // Pegasus instruction type
        InstType inst_type_ = InstType::INVALID; //!< Instruction type for instruction Events

        // PC
        bool is_change_of_flow_{false}; //!< True if this Event changed the PC (e.g. taken branches,
                                        //!< system calls, traps)
        Addr curr_pc_ = std::numeric_limits<Addr>::max(); //!< Current PC
        Addr next_pc_ = std::numeric_limits<Addr>::max(); //!< Next PC
        Addr alternate_next_pc_ =
            std::numeric_limits<Addr>::max(); //!< Alternate next PC if Event was not a change of
                                              //!< flow (e.g. branch or trap was not taken)

        // Privilege mode
        PrivMode curr_priv_ = PrivMode::INVALID; //!< Current privilege mode
        PrivMode next_priv_ = PrivMode::INVALID; //!< Next privilege mode

        // Load/Store Privilege mode
        PrivMode curr_ldst_priv_ = PrivMode::INVALID; //!< Current load/store privilege mode
        PrivMode next_ldst_priv_ = PrivMode::INVALID; //!< Next load/store privilege mode

        // Exceptions (traps and interrupts)
        ExcpType excp_type_ = ExcpType::INVALID; //!< The exception type for faulting instructions
                                                 //!< and interrupt Events
        ExcpCode excp_code_ =
            std::numeric_limits<ExcpCode>::max(); //!< The exception code for faulting instruction
                                                  //!< and interrupt Events
        ExcpCode prev_excp_code_ =
            std::numeric_limits<ExcpCode>::max(); //!< The previous exception code before this Event

        // Inst CSR which may cause side effects
        uint32_t inst_csr_ =
            std::numeric_limits<uint32_t>::max(); //!< The CSR accessed by the
                                                  //!< instruction causing this Event

        // PegasusState reservation
        sparta::utils::ValidValue<Addr>
            start_reservation_; //!< LR/SC reservation address (start of inst)
        sparta::utils::ValidValue<Addr>
            end_reservation_; //!< LR/SC reservation address (end of inst)

        // Softfloat state changes
        struct SoftfloatFlags
        {
            uint_fast8_t softfloat_roundingMode = 0;
            uint_fast8_t softfloat_detectTininess = 0;
            uint_fast8_t softfloat_exceptionFlags = 0;
            uint_fast8_t extF80_roundingPrecision = 0;

            template <typename Archive> void serialize(Archive & ar, const unsigned int /*version*/)
            {
                ar & softfloat_roundingMode;
                ar & softfloat_detectTininess;
                ar & softfloat_exceptionFlags;
                ar & extF80_roundingPrecision;
            }

            bool operator==(const SoftfloatFlags & other) const = default;
        };

        SoftfloatFlags start_softfloat_flags_;
        SoftfloatFlags end_softfloat_flags_;

        //! @}
        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \name Register Accesses
        //! @{

        std::vector<RegReadAccess> register_reads_;   //!< Registers read by this Event
        std::vector<RegWriteAccess> register_writes_; //!< Registers written by this Event

        //! @}
        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \name Memory Accesses
        //! @{

        std::vector<MemReadAccess> memory_reads_;   //!< Memory read by this Event
        std::vector<MemWriteAccess> memory_writes_; //!< Memory written by this Event

        //! @}
        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \name Extension changes
        //! @{

        struct MavisExtensions
        {
            std::vector<std::string> extensions;
            bool enabled;

            MavisExtensions(const std::vector<std::string> & extensions, bool enabled) :
                extensions(extensions),
                enabled(enabled)
            {
            }

            MavisExtensions() = default;

            bool operator==(const MavisExtensions & other) const = default;

            template <typename Archive> void serialize(Archive & ar, const unsigned int /*version*/)
            {
                ar & extensions;
                ar & enabled;
            }
        };

        std::vector<MavisExtensions> extension_changes_; //!< List of extension changes

        //! @}
        ////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \name Mavis Opcode Information
        //! @{

        std::string dasm_string_;

        //! @}
        ////////////////////////////////////////////////////////////////////////////////////////////

        /// Called to/from char buffer (boost::serialization)
        template <typename Archive> void serialize(Archive & ar, const unsigned int /*version*/)
        {
            ar & event_uid_;
            ar & type_;
            ar & core_id_;
            ar & hart_id_;
            ar & done_;
            ar & event_ends_sim_;
            ar & is_in_region_of_interest_;
            ar & is_entering_region_of_interest_;
            ar & is_exiting_region_of_interest_;
            ar & arch_id_;
            ar & opcode_;
            ar & opcode_size_;
            ar & inst_type_;
            ar & is_change_of_flow_;
            ar & curr_pc_;
            ar & next_pc_;
            ar & alternate_next_pc_;
            ar & curr_priv_;
            ar & next_priv_;
            ar & curr_ldst_priv_;
            ar & next_ldst_priv_;
            ar & excp_type_;
            ar & excp_code_;
            ar & prev_excp_code_;
            ar & inst_csr_;
            ar & start_reservation_;
            ar & end_reservation_;
            ar & start_softfloat_flags_;
            ar & end_softfloat_flags_;
            ar & register_reads_;
            ar & register_writes_;
            ar & memory_reads_;
            ar & memory_writes_;
            ar & extension_changes_;
            ar & dasm_string_;
        }

        friend class boost::serialization::access;
        friend class CoSimObserver;
        friend class CoSimEventPipeline;
        friend class EventCompressorStage;
    };

    inline std::ostream & operator<<(std::ostream & os, const Event::Type & type)
    {
        switch (type)
        {
            case pegasus::cosim::Event::Type::INSTRUCTION:
                os << "INSTRUCTION";
                break;
            case pegasus::cosim::Event::Type::INTERRUPT:
                os << "INTERRUPT";
                break;
            case pegasus::cosim::Event::Type::INJECTED_INSTRUCTION:
                os << "INJECTED_INSTRUCTION";
                break;
            case pegasus::cosim::Event::Type::INJECTED_INTERRUPT:
                os << "INJECTED_INTERRUPT";
                break;
            case pegasus::cosim::Event::Type::REGISTER_WRITE:
                os << "REGISTER_WRITE";
                break;
            case pegasus::cosim::Event::Type::MEMORY_WRITE:
                os << "MEMORY_WRITE";
                break;
            case pegasus::cosim::Event::Type::INVALID:
                os << "INVALID";
                break;
            default:
                os << "UNKNOWN";
                break;
        }
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os,
                                     const Event::RegReadAccess & reg_read_access)
    {
        os << reg_read_access.reg_id.reg_name << ": ";
        for (auto it = reg_read_access.value.rbegin(); it != reg_read_access.value.rend(); ++it)
        {
            const uint8_t & val = *it;
            os << std::setw(2) << std::setfill('0') << std::hex << (uint16_t)val;
        }
        os << "";
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os,
                                     const Event::RegWriteAccess & reg_write_access)
    {
        os << reg_write_access.reg_id.reg_name << ": ";
        for (auto it = reg_write_access.value.rbegin(); it != reg_write_access.value.rend(); ++it)
        {
            const uint8_t & val = *it;
            os << std::setw(2) << std::setfill('0') << std::hex << (uint16_t)val;
        }
        os << " [prev: ";
        for (auto it = reg_write_access.prev_value.rbegin();
             it != reg_write_access.prev_value.rend(); ++it)
        {
            const uint8_t & prev_val = *it;
            os << std::setw(2) << std::setfill('0') << std::hex << (uint16_t)prev_val;
        }
        os << "]";
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os,
                                     const std::vector<Event::RegReadAccess> & reg_read_accesses)
    {
        if (!reg_read_accesses.empty())
        {
            os << "Register Reads:" << std::endl;
            for (const auto & reg_read_access : reg_read_accesses)
            {
                os << " " << reg_read_access << "\n";
            }
        }
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os,
                                     const std::vector<Event::RegWriteAccess> & reg_write_accesses)
    {
        if (!reg_write_accesses.empty())
        {
            os << "Register Writes:" << std::endl;
            for (const auto & reg_write_access : reg_write_accesses)
            {
                os << " " << reg_write_access << "\n";
            }
        }
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const Event & event)
    {
        os << "euid: " << std::dec << event.getEuid() << " PC: " << std::setw(16)
           << std::setfill('0') << std::hex << event.getPc() << " " << event.getDisassemblyStr()
           << " (" << std::setw(8) << std::setfill('0') << std::hex << event.getOpcode() << ")"
           << "\n";
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const Event* event)
    {
        if (event)
        {
            os << *event;
        }
        else
        {
            os << "null";
        }
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os,
                                     const Event::MemReadAccess & mem_read_access)
    {
        os << "PAddr: " << std::setw(16) << std::setfill('0') << std::hex << mem_read_access.paddr
           << " VAddr: " << std::setw(16) << std::setfill('0') << std::hex << mem_read_access.vaddr
           << " Size: " << std::dec << mem_read_access.size << " Value: " << std::setw(16)
           << std::setfill('0') << std::hex
           << convertFromByteVector<uint64_t>(mem_read_access.value) << std::dec << "\n";

        return os;
    }

    inline std::ostream & operator<<(std::ostream & os,
                                     const Event::MemWriteAccess & mem_write_access)
    {
        os << "PAddr: " << std::setw(16) << std::setfill('0') << std::hex << mem_write_access.paddr
           << " VAddr: " << std::setw(16) << std::setfill('0') << std::hex << mem_write_access.vaddr
           << " Size: " << std::dec << mem_write_access.size << " Value: " << std::setw(16)
           << std::setfill('0') << std::hex
           << convertFromByteVector<uint64_t>(mem_write_access.value) << " [Prev: " << std::setw(16)
           << std::setfill('0') << std::hex
           << convertFromByteVector<uint64_t>(mem_write_access.prev_value) << std::dec << "]\n";

        return os;
    }

} // namespace pegasus::cosim
