#pragma once

#include <cstdint>

namespace atlas
{

    enum class TrapCauses : uint64_t
    {
        MISALIGNED_FETCH = 0x0,
        FETCH_ACCESS = 0x1,
        ILLEGAL_INSTRUCTION = 0x2,
        BREAKPOINT = 0x3,
        MISALIGNED_LOAD = 0x4,
        LOAD_ACCESS = 0x5,
        MISALIGNED_STORE = 0x6,
        STORE_ACCESS = 0x7,
        USER_ECALL = 0x8,
        SUPERVISOR_ECALL = 0x9,
        MACHINE_ECALL = 0xb,
        FETCH_PAGE_FAULT = 0xc,
        LOAD_PAGE_FAULT = 0xd,
        STORE_PAGE_FAULT = 0xf,
        SOFTWARE_CHECK_FAULT = 0x12,
        HARDWARE_ERROR_FAULT = 0x13,
    };

#define TRAP_IMPL(cause)                                                                           \
    {                                                                                              \
        auto exception_unit = state->getExceptionUnit();                                           \
        exception_unit->setUnhandledException(cause);                                              \
        return exception_unit->getActionGroup();                                                   \
    }

#define THROW_MISALIGNED_FETCH TRAP_IMPL(TrapCauses::MISALIGNED_FETCH)
#define THROW_FETCH_ACCESS TRAP_IMPL(TrapCauses::FETCH_ACCESS)
#define THROW_ILLEGAL_INSTRUCTION TRAP_IMPL(TrapCauses::ILLEGAL_INSTRUCTION)
#define THROW_BREAKPOINT TRAP_IMPL(TrapCauses::BREAKPOINT)
#define THROW_MISALIGNED_LOAD TRAP_IMPL(TrapCauses::MISALIGNED_LOAD)
#define THROW_LOAD_ACCESS TRAP_IMPL(TrapCauses::LOAD_ACCESS)
#define THROW_MISALIGNED_STORE TRAP_IMPL(TrapCauses::MISALIGNED_STORE)
#define THROW_STORE_ACCESS TRAP_IMPL(TrapCauses::STORE_ACCESS)
#define THROW_USER_ECALL TRAP_IMPL(TrapCauses::USER_ECALL)
#define THROW_SUPERVISOR_ECALL TRAP_IMPL(TrapCauses::SUPERVISOR_ECALL)
#define THROW_MACHINE_ECALL TRAP_IMPL(TrapCauses::MACHINE_ECALL)
#define THROW_FETCH_PAGE_FAULT TRAP_IMPL(TrapCauses::FETCH_PAGE_FAULT)
#define THROW_LOAD_PAGE_FAULT TRAP_IMPL(TrapCauses::LOAD_PAGE_FAULT)
#define THROW_STORE_PAGE_FAULT TRAP_IMPL(TrapCauses::STORE_PAGE_FAULT)
#define THROW_SOFTWARE_CHECK_FAULT TRAP_IMPL(TrapCauses::SOFTWARE_CHECK_FAULT)
#define THROW_HARDWARE_ERROR_FAULT TRAP_IMPL(TrapCauses::HARDWARE_ERROR_FAULT)

} // namespace atlas
