#pragma once
#include <bit>
#include <cstdint>
#include "core/inst_handlers/zknd/crypto-utils.hpp"

/*
    The different functors that are present here are :
        1. Aes32DsiOp
        2. Aes32DsmiOp
        3. Aes64DsOp
        4. Aes64DsmOp
        5. Aes64ImOp
        6. Aes64Ks1iOp
        7. Aes64Ks2Op
 */

namespace pegasus
{

    template <typename XLEN> struct Aes32DsiOp
    {
        XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const
        {
            const uint32_t bs = imm & 0x3;
            uint8_t byte = (rs2 >> (bs * 8)) & 0xFF;
            uint8_t sbox_out = aes_sbox_inv(byte);
            return rs1 ^ (std::rotl(static_cast<XLEN>(sbox_out), bs * 8));
        }
    };

    template <typename XLEN> struct Aes32DsmiOp
    {
        XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const
        {
            const uint32_t bs = imm & 0x3;
            uint8_t byte = (rs2 >> (bs * 8)) & 0xFF;
            uint8_t sbox_out = aes_sbox_inv(byte);
            uint32_t mixed = aes_mixcolumn_inv_byte(sbox_out);
            return rs1 ^ (std::rotl(static_cast<XLEN>(mixed), (bs * 8)));
        }
    };

    template <typename XLEN> struct Aes64DsOp
    {
        XLEN operator()(XLEN rs1, XLEN rs2, uint32_t /*imm*/) const
        {
            uint64_t shifted_low = aes_rv64_shiftrows_inv(rs2, rs1);
            uint64_t result = 0;
            for (int i = 0; i < 8; i++)
            {
                uint8_t byte = (shifted_low >> (i * 8)) & 0xFF;
                uint8_t inv_sbox = aes_sbox_inv(byte);
                result |= (uint64_t)inv_sbox << (i * 8);
            }
            return result;
        }
    };

    template <typename XLEN> struct Aes64DsmOp
    {
        XLEN operator()(XLEN rs1, XLEN rs2, uint32_t /*imm*/) const
        {
            uint64_t shifted_low = aes_rv64_shiftrows_inv(rs2, rs1);
            uint64_t subbed = 0;

            for (int i = 0; i < 8; i++)
            {
                uint8_t byte = (shifted_low >> (i * 8)) & 0xFF;
                uint8_t inv_sbox = aes_sbox_inv(byte);
                subbed |= (uint64_t)inv_sbox << (i * 8);
            }

            uint32_t low_col = (subbed & 0xFFFFFFFF);
            uint32_t high_col = (subbed >> 32);

            uint32_t mc_low = aes_mixcolumn_inv(low_col);
            uint32_t mc_high = aes_mixcolumn_inv(high_col);

            uint64_t mc_result = (static_cast<uint64_t>(mc_high) << 32) | mc_low;
            return mc_result;
        }
    };

    template <typename XLEN> struct Aes64ImOp
    {
        XLEN operator()(XLEN rs1, XLEN /*rs2*/, uint32_t /*imm*/) const
        {
            uint32_t w0 = aes_mixcolumn_inv(rs1 & 0xFFFFFFFF);
            uint32_t w1 = aes_mixcolumn_inv((rs1 >> 32) & 0xFFFFFFFF);
            return ((XLEN)w1 << 32) | (XLEN)w0;
        }
    };

    template <typename XLEN> struct Aes64Ks1iOp
    {
        XLEN operator()(XLEN rs1, XLEN /*rs2*/, uint32_t imm) const
        {
            const uint32_t rnum = imm & 0xF;

            uint32_t temp = (rs1 >> 32) & 0xFFFFFFFF;
            if (rnum != 0xA)
            {
                temp = std::rotr(temp, 8);
            }

            uint32_t subbed = 0;
            for (int i = 0; i < 4; i++)
            {
                uint8_t byte = (temp >> (i * 8)) & 0xFF;
                uint8_t sbox_out = aes_sbox_fwd(byte);
                subbed |= (uint32_t)sbox_out << (i * 8);
            }

            if (rnum < 10)
            {
                subbed ^= AES_RCON[rnum];
            }

            return ((XLEN)subbed << 32) | (XLEN)subbed;
        }
    };

    template <typename XLEN> struct Aes64Ks2Op
    {
        XLEN operator()(XLEN rs1, XLEN rs2, uint32_t /*imm*/) const
        {
            uint32_t w0 = (rs1 >> 32) ^ (rs2 & 0xFFFFFFFF);
            uint32_t w1 = (rs1 >> 32) ^ rs2 ^ (rs2 >> 32);
            return ((XLEN)w1 << 32) | (XLEN)w0;
        }
    };
} // namespace pegasus
