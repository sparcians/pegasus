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
        template <typename XLEN> ActionGroup* fadd_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fclass_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_lHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_luHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_wHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_wuHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_l_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_lu_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_w_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_wu_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fdiv_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* feq_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fle_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* flt_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmadd_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmax_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmin_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmsub_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmul_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_d_xHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_w_xHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_x_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_x_wHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fnmadd_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fnmsub_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnj_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnjn_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnjx_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsqrt_dHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsub_dHandler_(atlas::AtlasState* state);
    };
} // namespace atlas
