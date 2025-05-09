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
        Action* integer_reg_regHandler_(atlas::AtlasState* state, Action*);

        // integer reg-reg 32-bit operations
        Action* addwHandler_(atlas::AtlasState* state, Action*);
        Action* subwHandler_(atlas::AtlasState* state, Action*);

        // addi,slti,sltui,andi,ori,xori
        template <typename XLEN, typename OPERATOR>
        Action* integer_reg_immHandler_(atlas::AtlasState* state, Action*);

        // integer reg-imm 32-bit operations
        Action* addiwHandler_(atlas::AtlasState* state, Action*);

        // move and nop pseudo insts
        template <typename XLEN> Action* mvHandler_(atlas::AtlasState* state, Action*);
        Action* nopHandler_(atlas::AtlasState* state, Action*);

        // compute address for loads and stores
        template <typename XLEN, typename SIZE>
        Action* computeAddressHandler_(atlas::AtlasState* state, Action*);

        // lb,lbu,lh,lw,ld
        template <typename XLEN, typename SIZE, bool SIGN_EXTEND = false>
        Action* loadHandler_(atlas::AtlasState* state, Action*);

        // sb,sh,sw,sd
        template <typename XLEN, typename SIZE>
        Action* storeHandler_(atlas::AtlasState* state, Action*);

        // beq,bge,bgeu,blt,bltu,bne
        template <typename XLEN, typename OPERATOR>
        Action* branchHandler_(atlas::AtlasState* state, Action*);

        // jumps
        template <typename XLEN> Action* jalHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* jalrHandler_(atlas::AtlasState* state, Action*);

        // load imm, load upper imm, add upper imm to pc
        template <typename XLEN> Action* liHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* luiHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* auipcHandler_(atlas::AtlasState* state, Action*);

        // shifts,
        Action* sllHandler_(atlas::AtlasState* state, Action*);
        Action* slliHandler_(atlas::AtlasState* state, Action*);
        Action* slliwHandler_(atlas::AtlasState* state, Action*);
        Action* sllwHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* sraHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* sraiHandler_(atlas::AtlasState* state, Action*);
        Action* sraiwHandler_(atlas::AtlasState* state, Action*);
        Action* srawHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* srlHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* srliHandler_(atlas::AtlasState* state, Action*);
        Action* srliwHandler_(atlas::AtlasState* state, Action*);
        Action* srlwHandler_(atlas::AtlasState* state, Action*);

        // returns, environment calls, breakpoints, fences
        template <typename XLEN, PrivMode PRIV_MODE>
        Action* xretHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* ecallHandler_(atlas::AtlasState* state, Action*);
        Action* ebreakHandler_(atlas::AtlasState* state, Action*);
        Action* fenceHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* sfence_vmaHandler_(atlas::AtlasState* state, Action*);

        // wfi
        template <typename XLEN> Action* wfiHandler_(atlas::AtlasState* state, Action*);
    };
} // namespace atlas
