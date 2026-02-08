#include "core/inst_handlers/zknd/RvzkndInsts.hpp"
#include "core/PegasusState.hpp"
#include "include/ActionTags.hpp"
#include "include/IntNums.hpp"

#include <bit>

namespace pegasus
{
    // AES Inverse S-box lookup table - whisper table
    static constexpr uint8_t AES_SBOX_INV[256] = {
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7,
        0xfb, 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde,
        0xe9, 0xcb, 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42,
        0xfa, 0xc3, 0x4e, 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49,
        0x6d, 0x8b, 0xd1, 0x25, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c,
        0xcc, 0x5d, 0x65, 0xb6, 0x92, 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15,
        0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7,
        0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
        0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc,
        0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad,
        0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, 0x47, 0xf1, 0x1a, 0x71, 0x1d,
        0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, 0xfc, 0x56, 0x3e, 0x4b,
        0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, 0x1f, 0xdd, 0xa8,
        0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, 0x60, 0x51,
        0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 0xa0,
        0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c,
        0x7d
    };

    static constexpr uint8_t AES_RCON[10] = {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
    };

    template <typename XLEN>
    void RvzkndInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        if constexpr (sizeof(XLEN) == 4) 
        {
            inst_handlers.emplace(
                "aes32dsi",
                Action::createAction<&RvzkndInsts::aesHandler_<XLEN, Aes32DsiOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes32dsi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes32dsmi",
                Action::createAction<&RvzkndInsts::aesHandler_<XLEN, Aes32DsmiOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes32dsmi", ActionTags::EXECUTE_TAG));
        }
        else 
        {
            inst_handlers.emplace(
                "aes64ds",
                Action::createAction<&RvzkndInsts::aesHandler_<XLEN, Aes64DsOp<XLEN>>, RvzkndInsts>(
                    nullptr, "aes64ds", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes64dsm",
                Action::createAction<&RvzkndInsts::aesHandler_<XLEN, Aes64DsmOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes64dsm", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes64im",
                Action::createAction<&RvzkndInsts::aesHandler_<XLEN, Aes64ImOp<XLEN>>, RvzkndInsts>(
                    nullptr, "aes64im", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes64ks1i",
                Action::createAction<&RvzkndInsts::aesHandler_<XLEN, Aes64Ks1iOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes64ks1i", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes64ks2",
                Action::createAction<&RvzkndInsts::aesHandler_<XLEN, Aes64Ks2Op<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes64ks2", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzkndInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzkndInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RvzkndInsts::aesHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const auto & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        const uint32_t imm = inst->getImmediate();

        const XLEN rd_val = OPERATOR{}(rs1_val, rs2_val, imm);
        
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        return ++action_it;
    }


    uint8_t RvzkndInsts::aes_sbox_inv(uint8_t x) 
    { 
        return AES_SBOX_INV[x]; 
    }

    static constexpr uint8_t gf_mul(uint8_t a, uint8_t b)
    {
        uint8_t result = 0;
        uint8_t temp_a = a;
        uint8_t temp_b = b;

        for (int i = 0; i < 8; i++)
        {
            if (temp_b & 1)
                result ^= temp_a;
            
            bool hi_bit_set = (temp_a & 0x80) != 0;
            temp_a <<= 1;
            if (hi_bit_set)
                temp_a ^= 0x1b;            
            temp_b >>= 1;
        }
        return result;
    }

    static uint32_t aes_mixcolumn_inv(uint32_t col)
    {
        uint8_t s0 = (col >> 0) & 0xFF;
        uint8_t s1 = (col >> 8) & 0xFF;
        uint8_t s2 = (col >> 16) & 0xFF;
        uint8_t s3 = (col >> 24) & 0xFF;

        uint8_t b0 = gf_mul(s0, 0x0E) ^ gf_mul(s1, 0x0B) ^ gf_mul(s2, 0x0D) ^ gf_mul(s3, 0x09);
        uint8_t b1 = gf_mul(s0, 0x09) ^ gf_mul(s1, 0x0E) ^ gf_mul(s2, 0x0B) ^ gf_mul(s3, 0x0D);
        uint8_t b2 = gf_mul(s0, 0x0D) ^ gf_mul(s1, 0x09) ^ gf_mul(s2, 0x0E) ^ gf_mul(s3, 0x0B);
        uint8_t b3 = gf_mul(s0, 0x0B) ^ gf_mul(s1, 0x0D) ^ gf_mul(s2, 0x09) ^ gf_mul(s3, 0x0E);

        return (uint32_t)b0 | ((uint32_t)b1 << 8) | ((uint32_t)b2 << 16) | ((uint32_t)b3 << 24);
    }

    uint32_t RvzkndInsts::aes_mixcolumn_inv_byte(uint8_t byte)
    {
        uint8_t x0e = gf_mul(byte, 0x0E);
        uint8_t x09 = gf_mul(byte, 0x09);
        uint8_t x0d = gf_mul(byte, 0x0D);
        uint8_t x0b = gf_mul(byte, 0x0B);

        return (uint32_t)x0e | ((uint32_t)x09 << 8) | ((uint32_t)x0d << 16) | ((uint32_t)x0b << 24);
    }

    uint64_t RvzkndInsts::aes_rv64_shiftrows_inv(uint64_t rs2, uint64_t rs1)
    {
        auto get_byte = [](uint64_t val, int idx) -> uint8_t {
            return (val >> (idx * 8)) & 0xFF;
        };

        uint8_t result[8];
        result[0] = get_byte(rs1, 0);
        result[1] = get_byte(rs2, 5);
        result[2] = get_byte(rs2, 2);
        result[3] = get_byte(rs1, 7);
        result[4] = get_byte(rs1, 4);
        result[5] = get_byte(rs1, 1);
        result[6] = get_byte(rs2, 6);
        result[7] = get_byte(rs2, 3);

        uint64_t output = 0;
        for (int i = 0; i < 8; i++)
        {
            output |= (uint64_t)result[i] << (i * 8);
        }
        return output;
    }


    // 32 bit :  final round decrypt
    template <typename XLEN>
    XLEN RvzkndInsts::Aes32DsiOp<XLEN>::operator()(XLEN rs1, XLEN rs2, uint32_t imm) const
    {
        const uint32_t bs = imm & 0x3;  // 2-bit byte select
        uint8_t byte = (rs2 >> (bs * 8)) & 0xFF;
        uint8_t sbox_out = RvzkndInsts::aes_sbox_inv(byte);
        return rs1 ^ (static_cast<XLEN>(sbox_out) << (bs * 8));
    }

    // 32 bit function : middle round decrypt
    template <typename XLEN>
    XLEN RvzkndInsts::Aes32DsmiOp<XLEN>::operator()(XLEN rs1, XLEN rs2, uint32_t imm) const
    {
        const uint32_t bs = imm & 0x3;
        uint8_t byte = (rs2 >> (bs * 8)) & 0xFF;
        uint8_t sbox_out = RvzkndInsts::aes_sbox_inv(byte);
        uint32_t mixed = RvzkndInsts::aes_mixcolumn_inv_byte(sbox_out);
        return rs1 ^ (static_cast<XLEN>(mixed) << (bs * 8));
    }

    template <typename XLEN>
    XLEN RvzkndInsts::Aes64DsOp<XLEN>::operator()(XLEN rs1, XLEN rs2, uint32_t /*imm*/) const
    {
        uint64_t shifted_low = aes_rv64_shiftrows_inv(rs2, rs1);
        uint64_t result = 0;
        for(int i = 0; i < 8; i++){
            uint8_t byte = (shifted_low >> (i * 8)) & 0xFF;
            uint8_t inv_sbox = aes_sbox_inv(byte);
            result |= inv_sbox << (i * 8);

        }
        return result;
    }

    //aes  decrypt middle round
    template <typename XLEN>
    XLEN RvzkndInsts::Aes64DsmOp<XLEN>::operator()(XLEN rs1, XLEN rs2, uint32_t /*imm*/) const
    {
       uint64_t shifted_low = aes_rv64_shiftrows_inv(rs2, rs1);
       uint64_t subbed = 0;

       for(int i = 0; i < 8; i++){
           uint8_t byte = (shifted_low >> (i * 8)) & 0xFF;
           uint8_t inv_sbox = aes_sbox_inv(byte);
           subbed |= inv_sbox << (i * 8);
       }

       uint32_t low_col = (  subbed & 0xFFFFFFFF);
       uint32_t high_col = (subbed >> 32);

       uint32_t mc_low = aes_mixcolumn_inv(low_col);
       uint32_t mc_high = aes_mixcolumn_inv(high_col);

       uint64_t mc_result =( static_cast<uint64_t>(mc_high) << 32) | mc_low;
       return mc_result;
    }

    // aes  decrypt key schedule inverse MixColumns
    template <typename XLEN>
    XLEN RvzkndInsts::Aes64ImOp<XLEN>::operator()(XLEN rs1, XLEN /*rs2*/, uint32_t /*imm*/) const
    {
        uint32_t w0 = aes_mixcolumn_inv(rs1 & 0xFFFFFFFF);
        uint32_t w1 = aes_mixcolumn_inv((rs1 >> 32) & 0xFFFFFFFF);
        return ((XLEN)w1 << 32) | (XLEN)w0;
    }

    // aes - 64 bit  key schedule instruction 1
    template <typename XLEN>
    XLEN RvzkndInsts::Aes64Ks1iOp<XLEN>::operator()(XLEN rs1, XLEN /*rs2*/, uint32_t imm) const
    {
        const uint32_t rnum = imm & 0xF;
        
        uint32_t temp = (rs1 >> 32) & 0xFFFFFFFF;
        
        if (rnum != 0xA) {
            temp = std::rotr(temp, 8); 
        }
        
        uint32_t subbed = 0;
        for (int i = 0; i < 4; i++)
        {
            uint8_t byte = (temp >> (i * 8)) & 0xFF;
            uint8_t sbox_out = RvzkndInsts::aes_sbox_inv(byte);
            subbed |= (uint32_t)sbox_out << (i * 8);
        }
        
        if (rnum < 10)
        {
            subbed ^= AES_RCON[rnum];
        }
        
        return ((XLEN)subbed << 32) | (XLEN)subbed;
    }

    // aes key schedule instruction 2
    template <typename XLEN>
    XLEN RvzkndInsts::Aes64Ks2Op<XLEN>::operator()(XLEN rs1, XLEN rs2, uint32_t /*imm*/) const
    {
        uint32_t w0 = (rs1 >> 32) ^ (rs2 & 0xFFFFFFFF);
        uint32_t w1 = (rs1 >> 32) ^ w0 ^ ((rs2 >> 32) & 0xFFFFFFFF);
        return ((XLEN)w1 << 32) | (XLEN)w0;
    }

    template Action::ItrType
    RvzkndInsts::aesHandler_<RV32, RvzkndInsts::Aes32DsiOp<RV32>>(PegasusState*, Action::ItrType);

    template Action::ItrType
    RvzkndInsts::aesHandler_<RV32, RvzkndInsts::Aes32DsmiOp<RV32>>(PegasusState*, Action::ItrType);

    template Action::ItrType
    RvzkndInsts::aesHandler_<RV64, RvzkndInsts::Aes64DsOp<RV64>>(PegasusState*, Action::ItrType);

    template Action::ItrType
    RvzkndInsts::aesHandler_<RV64, RvzkndInsts::Aes64DsmOp<RV64>>(PegasusState*, Action::ItrType);

    template Action::ItrType
    RvzkndInsts::aesHandler_<RV64, RvzkndInsts::Aes64ImOp<RV64>>(PegasusState*, Action::ItrType);

    template Action::ItrType
    RvzkndInsts::aesHandler_<RV64, RvzkndInsts::Aes64Ks1iOp<RV64>>(PegasusState*, Action::ItrType);

    template Action::ItrType
    RvzkndInsts::aesHandler_<RV64, RvzkndInsts::Aes64Ks2Op<RV64>>(PegasusState*, Action::ItrType);

    template struct RvzkndInsts::Aes32DsiOp<RV32>;
    template struct RvzkndInsts::Aes32DsmiOp<RV32>;
    template struct RvzkndInsts::Aes64DsOp<RV64>;
    template struct RvzkndInsts::Aes64DsmOp<RV64>;
    template struct RvzkndInsts::Aes64ImOp<RV64>;
    template struct RvzkndInsts::Aes64Ks1iOp<RV64>;
    template struct RvzkndInsts::Aes64Ks2Op<RV64>;

}
