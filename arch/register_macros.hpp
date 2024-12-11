#pragma once

#include "core/inst_handlers/inst_helpers.hpp"

#define READ_INT_REG(reg_name)                                                                     \
    (atlas::INT::reg_name::reg_num == 0)                                                           \
        ? 0                                                                                        \
        : state->getIntRegister(atlas::INT::reg_name::reg_num)->dmiRead<uint64_t>()

#define WRITE_INT_REG(reg_name, reg_value)                                                         \
    if constexpr (atlas::INT::reg_name::reg_num != 0)                                              \
        state->getIntRegister(atlas::INT::reg_name::reg_num)->dmiWrite<uint64_t>(reg_value);

#define READ_FP_FIELD(reg_name, field_name)                                                        \
    (atlas::FP::reg_name::reg_num == 1)                                                            \
        ? state->getFpRegister(atlas::FP::reg_name::reg_num)->dmiRead<uint64_t>()                  \
        : state->getFpRegister(atlas::FP::reg_name::reg_num)->dmiRead<uint64_t>()                  \
              & 0x00000000ffffffff

#define WRITE_FP_FIELD(reg_name, field_name, field_value)                                          \
    if constexpr (atlas::FP::reg_name::reg_num == 1)                                               \
    {                                                                                              \
        state->getFpRegister(atlas::FP::reg_name::reg_num)->dmiWrite<uint64_t>(field_value);       \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        state->getFpRegister(atlas::FP::reg_name::reg_num)                                         \
            ->dmiWrite<uint64_t>(field_value | 0xffffffff00000000);                                \
    }

#define READ_VEC_REG(reg_name)                                                                     \
    state->getVecRegister(atlas::VEC::reg_name::reg_num)->dmiRead<uint64_t>()

#define WRITE_VEC_REG(reg_name, reg_value)                                                         \
    state->getVecRegister(atlas::VEC::reg_name::reg_num)->dmiWrite(reg_value);

#define READ_CSR_REG(reg_name)                                                                     \
    state->getCsrRegister(atlas::CSR::reg_name::reg_num)->dmiRead<uint64_t>()

#define WRITE_CSR_REG(reg_name, reg_value)                                                         \
    if constexpr (atlas::CSR::reg_name::writable_fields_bit_mask != 0xffffffffffffffff)            \
    {                                                                                              \
        const auto old_value =                                                                     \
            state->getCsrRegister(atlas::CSR::reg_name::reg_num)->dmiRead<uint64_t>();             \
        state->getCsrRegister(atlas::CSR::reg_name::reg_num)                                       \
            ->dmiWrite(                                                                            \
                atlas::RegisterBitMask<atlas::CSR::reg_name::writable_fields_bit_mask>::mask(      \
                    old_value, reg_value));                                                        \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        state->getCsrRegister(atlas::CSR::reg_name::reg_num)->dmiWrite(reg_value);                 \
    }

#define READ_CSR_FIELD(reg_name, field_name)                                                       \
    atlas::CSRFields<atlas::CSR::reg_name::field_name>::readField(                                 \
        state->getCsrRegister(atlas::CSR::reg_name::reg_num)->dmiRead<uint64_t>())

#define WRITE_CSR_FIELD(reg_name, field_name, field_value)                                         \
    {                                                                                              \
        const auto old_value =                                                                     \
            state->getCsrRegister(atlas::CSR::reg_name::reg_num)->dmiRead<uint64_t>();             \
        WRITE_CSR_REG(reg_name,                                                                    \
                      (old_value | (field_value << atlas::CSR::reg_name::field_name::low_bit)));   \
    }

#define PEEK_CSR_REG(reg_name) READ_CSR_REG(reg_name)

#define POKE_CSR_REG(reg_name, reg_value)                                                          \
    state->getCsrRegister(atlas::CSR::reg_name::reg_num)->dmiWrite(reg_value);
