#pragma once

#include <map>
#include <string>
#include "core/inst_handlers/f/RvfInstsBase.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvdInsts : public RvfInstsBase
    {
      public:
        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN> Action* fadd_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fclass_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_d_lHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_d_luHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_d_sHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_d_wHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_d_wuHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_l_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_lu_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_s_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_w_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fcvt_wu_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fdiv_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* feq_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fle_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* flt_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmadd_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmax_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmin_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmsub_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmul_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmv_d_xHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmv_w_xHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmv_x_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fmv_x_wHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fnmadd_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fnmsub_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsgnj_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsgnjn_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsgnjx_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsqrt_dHandler_(atlas::AtlasState* state, Action*);
        template <typename XLEN> Action* fsub_dHandler_(atlas::AtlasState* state, Action*);
    };
} // namespace atlas
