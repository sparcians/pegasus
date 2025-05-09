#pragma once

#include <map>
#include <string>
#include "core/inst_handlers/f/RvfInstsBase.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvfInsts : public RvfInstsBase
    {
      public:
        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN> Action* fadd_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fclass_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_l_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_lu_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_s_lHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_s_luHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_s_wHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_s_wuHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_w_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_wu_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fdiv_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* feq_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fle_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* flt_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmadd_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmax_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmin_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmsub_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmul_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmv_w_xHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmv_x_wHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fnmadd_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fnmsub_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsgnj_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsgnjn_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsgnjx_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsqrt_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsub_sHandler_(atlas::AtlasState* state, Action*);
    };
} // namespace atlas
