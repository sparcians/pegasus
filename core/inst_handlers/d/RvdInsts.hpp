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
        template <typename XLEN>
        Action::ItrType fadd_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fclass_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_d_lHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_d_luHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_d_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_d_wHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_d_wuHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_l_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_lu_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_s_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_w_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_wu_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fdiv_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType feq_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fle_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType flt_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmadd_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmax_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmin_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmsub_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmul_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmv_d_xHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmv_w_xHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmv_x_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmv_x_wHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fnmadd_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fnmsub_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsgnj_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsgnjn_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsgnjx_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsqrt_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsub_dHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
