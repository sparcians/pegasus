#pragma once
#include <bit>
#include <cstdint>
#include "core/inst_handlers/zknd/crypto-utils.hpp"

namespace pegasus
{
    template <typename XLEN> struct Aes32esHandler
    {
        XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const 
        {
            const uint32_t bs = imm & 0x3;
            const uint8_t shamt = bs << 3;
            const uint8_t si = (static_cast<uint32_t>(rs2) >> shamt) & 0xFF;
            const uint32_t so = static_cast<uint32_t>(aes_sbox_fwd(si));
            const uint32_t result = static_cast<uint32_t>(rs1) ^ std::rotl(so, shamt);
            return static_cast<XLEN>(static_cast<int32_t>(result));
        }
    };

    template <typename XLEN> struct Aes32esmHandler
    {
        XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const
        {
            const uint32_t bs = imm & 0x3;
            const uint8_t shamt = bs << 3;
            const uint8_t si = (static_cast<uint32_t>(rs2) >> shamt) & 0xFF;
            const uint32_t so = aes_sbox_fwd(si);
            const uint32_t mixed = aes_mixcolumn_byte_fwd(so);
            const XLEN result = static_cast<XLEN>(rs1) ^ std::rotl(mixed, shamt);
            return static_cast<XLEN>(static_cast<uint32_t>(result));
        }
    };

    template <typename XLEN> struct Aes64esHandler
    {
        XLEN operator()(XLEN rs1, XLEN rs2, uint32_t /*imm*/) const
        {
            const XLEN sr = aes_rv64_shiftrows_fwd(rs2, rs1);
            const XLEN wd = sr;
            const XLEN result = aes_apply_fwd_sbox_to_each_byte(wd);
            return result;
        }
    };

    

    template<typename XLEN> struct Aes64esmHandler
    {
        XLEN operator()(XLEN rs1, XLEN rs2, uint32_t /*imm*/)
        {
            const XLEN  sr = aes_rv64_shiftrows_fwd(rs2, rs1);
            const XLEN wd = sr;
            const XLEN sb = aes_apply_fwd_sbox_to_each_byte(wd);
            const uint32_t lower_half = static_cast<uint32_t>(sb & 0xFFFFFFFF);
            const uint32_t upper_half = static_cast<uint32_t>(sb >> 32);

            const XLEN result = static_cast<uint64_t>(aes_mixcolumn_fwd(upper_half)) << 32 | aes_mixcolumn_fwd(lower_half);

            return result;
        }
    };
}