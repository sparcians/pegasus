#pragma once

#include <cinttypes>
#include <string>
#include <ostream>
#include <array>
#include <boost/int128/int128.hpp>

namespace pegasus
{
    using Addr = uint64_t;
    using Opcode = uint32_t;
    using OpcodeSize = size_t;
    using CoreId = uint32_t;
    using HartId = uint32_t;
    using ExcpCode = uint64_t;

    using ActionTagType = uint32_t;

    using RV32 = uint32_t;
    using RV64 = uint64_t;

    using B = uint8_t;
    using H = uint16_t;
    using W = uint32_t;
    using D = uint64_t;
    using Q = boost::int128::uint128_t;

    using FLOAT_HP = uint16_t;
    using FLOAT_SP = uint32_t;
    using FLOAT_DP = uint64_t;

    enum class SimPauseReason
    {
        QUANTUM,   //! Instruction quantum reached
        INTERRUPT, //! Interrupt
        PAUSE,     //! Pause
        FORK,      //! New thread
        WRS_NTO,   //! Wait on reservation set, with no timeout
        WRS_STO,   //! Wait on reservation set, with short timeout
        INVALID    //! Invalid
    };

    enum class InstType
    {
        SCALAR,         //! Scalar (integer)
        FLOATING_POINT, //! Floating point
        VECTOR,         //! Vector
        BRANCH,         //! Branch
        LOAD,           //! Load
        STORE,          //! Store
        STORECOND,      //! Atomic store conditional
        MISC,           //! Miscellaneous (csr, ecall, wfi, etc.)
        INVALID         //! Invalid
    };

    enum class RegType
    {
        INTEGER = 1,
        FLOATING_POINT = 2,
        VECTOR = 3,
        CSR = 4,
        INVALID
    };

    enum class MemAccessSource
    {
        FETCH,       //! Memory access from fetching an instruction
        INSTRUCTION, //! Memory access made by an instruction (load or store)
        HARDWARE,    //! Memory access made by hardware (e.g. page table walk)
        INVALID
    };

    enum class PrivMode
    {
        USER = 0,
        SUPERVISOR = 1,
        HYPERVISOR = 2,
        MACHINE = 3,
        INVALID
    };

    static constexpr uint32_t N_PRIV_MODES = static_cast<uint32_t>(PrivMode::INVALID);

    enum class ExcpType
    {
        FAULT,       //! Trap caused by unexpected program behavior
        SYSTEM_CALL, //! Trap caused by an ecall instruction
        INTERRUPT,   //! Asynchronous exception
        INVALID
    };

    enum class TrapVectorMode
    {
        DIRECT = 0,   //! All traps set pc to BASE.
        VECTORED = 1, //! Asynchronous interrupts set pc to BASE+4×cause
        RESERVED
    };

    struct RegId
    {
        RegType reg_type;
        uint32_t reg_num;
        std::string reg_name;

        /// Called to/from char buffer (boost::serialization)
        template <typename Archive> void serialize(Archive & ar, const unsigned int /*version*/)
        {
            ar & reg_type;
            ar & reg_num;
            ar & reg_name;
        }

        bool operator==(const RegId & other) const = default;
    };

    enum class RoundingMode
    {
        EVEN = 0,
        MINMAG = 1,
        MIN = 2,
        MAX = 3,
        MAXMAG = 4,
        ODD = 6,
        DYN
    };

    inline std::ostream & operator<<(std::ostream & os, const MemAccessSource source)
    {
        switch (source)
        {
            case MemAccessSource::FETCH:
                os << "FETCH";
                break;
            case MemAccessSource::INSTRUCTION:
                os << "INSTRUCTION";
                break;
            case MemAccessSource::HARDWARE:
                os << "HARDWARE";
                break;
            case MemAccessSource::INVALID:
                os << "INVALID";
                break;
        }
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const PrivMode mode)
    {
        switch (mode)
        {
            case PrivMode::USER:
                os << "U";
                break;
            case PrivMode::SUPERVISOR:
                os << "S";
                break;
            case PrivMode::HYPERVISOR:
                os << "H";
                break;
            case PrivMode::MACHINE:
                os << "M";
                break;
            case PrivMode::INVALID:
                os << "INVALID";
                break;
        }
        return os;
    }

    // Common opcodes
    constexpr uint64_t WFI_OPCODE = 0x10500073;
    constexpr uint64_t NOP_OPCODE = 0x00000013;
    constexpr uint64_t ECALL_OPCODE = 0x00000073;

    // System Call emulation
    using SystemCallStack = std::array<uint64_t, 8>;

} // namespace pegasus
