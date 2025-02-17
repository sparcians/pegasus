#pragma once

#include "core/inst_handlers/inst_helpers.hpp"
#include "include/CSRNums.hpp"
#include "core/IntNums.hpp"
#include "core/FpNums.hpp"
#include "core/VecNums.hpp"
#include "include/CSRHelpers.hpp"
#include <utility>
#include <vector>
#include <unordered_map>
#include <string>

#define READ_CSR_FIELD(reg_ident, field_name)                                                      \
    ((state->getCsrRegister(reg_ident)->dmiRead<uint64_t>()                                        \
      >> atlas::getCsrBitRange(reg_ident, #field_name).first)                                      \
     & ((1ULL << (atlas::getCsrBitRange(reg_ident, #field_name).second                             \
                  - atlas::getCsrBitRange(reg_ident, #field_name).first + 1))                      \
        - 1))

#define WRITE_CSR_FIELD(reg_ident, field_name, field_value)                                        \
    {                                                                                              \
        auto csr_value = state->getCsrRegister(reg_ident)->dmiRead<uint64_t>();                    \
                                                                                                   \
        const auto low_bit = atlas::getCsrBitRange(reg_ident, #field_name).first;                  \
        const auto high_bit = atlas::getCsrBitRange(reg_ident, #field_name).second;                \
        const auto mask = ((1ULL << (high_bit - low_bit + 1)) - 1) << low_bit;                     \
        csr_value &= ~mask;                                                                        \
                                                                                                   \
        const uint64_t new_field_value = field_value << low_bit;                                   \
        csr_value |= new_field_value;                                                              \
                                                                                                   \
        state->getCsrRegister(reg_ident)->write(csr_value);                                        \
    }

#define POKE_CSR_FIELD(reg_ident, field_name, field_value)                                         \
    {                                                                                              \
        auto csr_value = state->getCsrRegister(reg_ident)->dmiRead<uint64_t>();                    \
                                                                                                   \
        const auto low_bit = atlas::getCsrBitRange(reg_ident, #field_name).first;                  \
        const auto high_bit = atlas::getCsrBitRange(reg_ident, #field_name).second;                \
        const auto mask = ((1ULL << (high_bit - low_bit + 1)) - 1) << low_bit;                     \
        csr_value &= ~mask;                                                                        \
                                                                                                   \
        const uint64_t new_field_value = (uint64_t)field_value << low_bit;                         \
        csr_value |= new_field_value;                                                              \
                                                                                                   \
        state->getCsrRegister(reg_ident)->dmiWrite(csr_value);                                     \
    }
