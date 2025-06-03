#pragma once

#include "include/AtlasTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;

    class RviInsts
    {
      public:
        using base_type = RviInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> &);
        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &, bool);

      private:
        // add,slt,sltu,and,or,xor,sub
        template <typename XLEN, typename OPERATOR>
        Action::ItrType integer_reg_regHandler_(atlas::AtlasState* state,
                                                Action::ItrType action_it);

        // integer reg-reg 32-bit operations
        Action::ItrType addwHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType subwHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // addi,slti,sltui,andi,ori,xori
        template <typename XLEN, typename OPERATOR>
        Action::ItrType integer_reg_immHandler_(atlas::AtlasState* state,
                                                Action::ItrType action_it);

        // integer reg-imm 32-bit operations
        Action::ItrType addiwHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // move and nop pseudo insts
        template <typename XLEN>
        Action::ItrType mvHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType nopHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // compute address for loads and stores
        template <typename XLEN, typename SIZE>
        Action::ItrType computeAddressHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // lb,lbu,lh,lw,ld
        template <typename XLEN, typename SIZE, bool SIGN_EXTEND = false>
        Action::ItrType loadHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // sb,sh,sw,sd
        template <typename XLEN, typename SIZE>
        Action::ItrType storeHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // beq,bge,bgeu,blt,bltu,bne
        template <typename XLEN, typename OPERATOR>
        Action::ItrType branchHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // jumps
        template <typename XLEN>
        Action::ItrType jalHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType jalrHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // load imm, load upper imm, add upper imm to pc
        template <typename XLEN>
        Action::ItrType liHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType luiHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType auipcHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // shifts,
        Action::ItrType sllHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType slliHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType slliwHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType sllwHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType sraHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType sraiHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType sraiwHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType srawHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType srlHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType srliHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType srliwHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType srlwHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // returns, environment calls, breakpoints, fences
        template <typename XLEN, PrivMode PRIV_MODE>
        Action::ItrType xretHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType ecallHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType ecallHandlerSystemEmulation_(atlas::AtlasState* state,
                                                     Action::ItrType action_it);
        Action::ItrType ebreakHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        Action::ItrType fenceHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType sfence_vmaHandler_(atlas::AtlasState* state, Action::ItrType action_it);

        // wfi
        template <typename XLEN>
        Action::ItrType wfiHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
