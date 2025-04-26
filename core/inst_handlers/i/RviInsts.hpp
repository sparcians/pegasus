#pragma once

#include "include/AtlasTypes.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RviInsts
    {
      public:
        using base_type = RviInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> &);
        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:
        // add,slt,sltu,and,or,xor,sub
        template <typename XLEN, typename OPERATOR>
        ActionGroup* integer_reg_regHandler_(atlas::AtlasState* state);

        // integer reg-reg 32-bit operations
        ActionGroup* addwHandler_(atlas::AtlasState* state);
        ActionGroup* subwHandler_(atlas::AtlasState* state);

        // addi,slti,sltui,andi,ori,xori
        template <typename XLEN, typename OPERATOR>
        ActionGroup* integer_reg_immHandler_(atlas::AtlasState* state);

        // integer reg-imm 32-bit operations
        ActionGroup* addiwHandler_(atlas::AtlasState* state);

        // move and nop pseudo insts
        template <typename XLEN> ActionGroup* mvHandler_(atlas::AtlasState* state);
        ActionGroup* nopHandler_(atlas::AtlasState* state);

        // compute address for loads and stores
        template <typename XLEN, typename SIZE>
        ActionGroup* computeAddressHandler_(atlas::AtlasState* state);

        // lb,lbu,lh,lw,ld
        template <typename XLEN, typename SIZE, bool SIGN_EXTEND = false>
        ActionGroup* loadHandler_(atlas::AtlasState* state);

        // sb,sh,sw,sd
        template <typename XLEN, typename SIZE>
        ActionGroup* storeHandler_(atlas::AtlasState* state);

        // beq,bge,bgeu,blt,bltu,bne
        template <typename XLEN, typename OPERATOR>
        ActionGroup* branchHandler_(atlas::AtlasState* state);

        // jumps
        template <typename XLEN> ActionGroup* jalHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* jalrHandler_(atlas::AtlasState* state);

        // load imm, load upper imm, add upper imm to pc
        template <typename XLEN> ActionGroup* liHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* luiHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* auipcHandler_(atlas::AtlasState* state);

        // shifts,
        ActionGroup* sllHandler_(atlas::AtlasState* state);
        ActionGroup* slliHandler_(atlas::AtlasState* state);
        ActionGroup* slliwHandler_(atlas::AtlasState* state);
        ActionGroup* sllwHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* sraHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* sraiHandler_(atlas::AtlasState* state);
        ActionGroup* sraiwHandler_(atlas::AtlasState* state);
        ActionGroup* srawHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* srlHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* srliHandler_(atlas::AtlasState* state);
        ActionGroup* srliwHandler_(atlas::AtlasState* state);
        ActionGroup* srlwHandler_(atlas::AtlasState* state);

        // returns, environment calls, breakpoints, fences
        template <typename XLEN, PrivMode PRIV_MODE>
        ActionGroup* xretHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* ecallHandler_(atlas::AtlasState* state);
        ActionGroup* ebreakHandler_(atlas::AtlasState* state);
        ActionGroup* fenceHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* sfence_vmaHandler_(atlas::AtlasState* state);

        // wfi
        template <typename XLEN> ActionGroup* wfiHandler_(atlas::AtlasState* state);
    };
} // namespace atlas
