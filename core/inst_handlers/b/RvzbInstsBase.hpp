#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"
#include "core/PegasusState.hpp"

#include <map>
#include <string>
#include <bit>

namespace pegasus
{
    class PegasusState;

    class RvzbInstsBase
    {
      public:
        using base_type = RvzbInstsBase;

        virtual ~RvzbInstsBase() = default;

      protected:
        template <typename XLEN, typename OP>
        Action::ItrType unaryOpHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename OP>
        Action::ItrType binaryOpHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN, typename OP>
        Action::ItrType immOpHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };

    template <typename XLEN, typename OP>
    Action::ItrType RvzbInstsBase::unaryOpHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        const XLEN rd_val = OP()(rs1_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN, typename OP>
    Action::ItrType RvzbInstsBase::binaryOpHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = OP()(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN, typename OP>
    Action::ItrType RvzbInstsBase::immOpHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        const XLEN rd_val = OP()(rs1_val, imm_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

} // namespace pegasus
