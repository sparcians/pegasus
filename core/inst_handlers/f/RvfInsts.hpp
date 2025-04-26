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
        template <typename XLEN> ActionGroup* fadd_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fclass_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_l_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_lu_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_lHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_luHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_wHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_wuHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_w_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_wu_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fdiv_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* feq_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fle_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* flt_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmadd_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmax_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmin_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmsub_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmul_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_w_xHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_x_wHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fnmadd_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fnmsub_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnj_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnjn_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnjx_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsqrt_sHandler_(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsub_sHandler_(atlas::AtlasState* state);
    };
} // namespace atlas
