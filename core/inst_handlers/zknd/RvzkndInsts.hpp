#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzkndInsts
    {
      public:
        using base_type = RvzkndInsts;

        template <typename XLEN>
        static void getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers);

      private:
        template <typename XLEN, typename OPERATOR>
        Action::ItrType aesHandler_(PegasusState* state, Action::ItrType action_it);

        template <typename XLEN> struct Aes32DsiOp
        {
            XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const;
        };

        template <typename XLEN> struct Aes32DsmiOp
        {
            XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const;
        };

        template <typename XLEN> struct Aes64DsOp
        {
            XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const;
        };

        template <typename XLEN> struct Aes64DsmOp
        {
            XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const;
        };

        template <typename XLEN> struct Aes64ImOp
        {
            XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const;
        };

        template <typename XLEN> struct Aes64Ks1iOp
        {
            XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const;
        };

        template <typename XLEN> struct Aes64Ks2Op
        {
            XLEN operator()(XLEN rs1, XLEN rs2, uint32_t imm) const;
        };

        static uint8_t aes_sbox_inv(uint8_t x);
        static uint32_t aes_mixcolumn_inv_byte(uint8_t byte);
        static uint64_t aes_rv64_shiftrows_inv(uint64_t rs2, uint64_t rs1);
    };

} 
