#include "core/inst_handlers/m/RvmInsts.hpp"
#include "core/inst_handlers/m/RvmFunctors.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"

#include <functional>

namespace pegasus
{
    template <typename XLEN>
    void RvmInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace("div",
                              pegasus::Action::createAction<&RvmInsts::divHandler_<XLEN>, RvmInsts>(
                                  nullptr, "div", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "divu", pegasus::Action::createAction<&RvmInsts::divuHandler_<XLEN>, RvmInsts>(
                        nullptr, "divu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "mul",
            pegasus::Action::createAction<&RvmInsts::mulHandler_<XLEN, std::multiplies<XLEN>>,
                                          RvmInsts>(nullptr, "mul", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "mulh",
            pegasus::Action::createAction<&RvmInsts::mulHandler_<XLEN, Mulh<XLEN>>, RvmInsts>(
                nullptr, "mulh", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "mulhsu",
            pegasus::Action::createAction<&RvmInsts::mulHandler_<XLEN, Mulhsu<XLEN>>, RvmInsts>(
                nullptr, "mulhsu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "mulhu",
            pegasus::Action::createAction<&RvmInsts::mulHandler_<XLEN, Mulhu<XLEN>>, RvmInsts>(
                nullptr, "mulhu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("rem",
                              pegasus::Action::createAction<&RvmInsts::remHandler_<XLEN>, RvmInsts>(
                                  nullptr, "rem", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "remu", pegasus::Action::createAction<&RvmInsts::remuHandler_<XLEN>, RvmInsts>(
                        nullptr, "remu", ActionTags::EXECUTE_TAG));

        // RV64 only 32-bit instructions
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace("divuw",
                                  pegasus::Action::createAction<&RvmInsts::divuwHandler_, RvmInsts>(
                                      nullptr, "divuw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("divw",
                                  pegasus::Action::createAction<&RvmInsts::divwHandler_, RvmInsts>(
                                      nullptr, "divw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("mulw",
                                  pegasus::Action::createAction<&RvmInsts::mulwHandler_, RvmInsts>(
                                      nullptr, "mulw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("remuw",
                                  pegasus::Action::createAction<&RvmInsts::remuwHandler_, RvmInsts>(
                                      nullptr, "remuw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("remw",
                                  pegasus::Action::createAction<&RvmInsts::remwHandler_, RvmInsts>(
                                      nullptr, "remw", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvmInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvmInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN>
    Action::ItrType RvmInsts::divHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        sreg_t lhs = sext(rs1_val, state->getXlen());
        sreg_t rhs = sext(rs2_val, state->getXlen());

        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), UINT64_MAX);
        }
        else if (lhs == INT64_MIN && rhs == -1)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), lhs);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext((lhs / rhs), state->getXlen()));
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvmInsts::divuHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        reg_t lhs = zext(rs1_val, state->getXlen());
        reg_t rhs = zext(rs2_val, state->getXlen());
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), UINT64_MAX);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext((lhs / rhs), state->getXlen()));
        }

        return ++action_it;
    }

    Action::ItrType RvmInsts::divuwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using XLEN = uint64_t;
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        reg_t lhs = zext32(rs1_val);
        reg_t rhs = zext32(rs2_val);
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), UINT64_MAX);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext32(lhs / rhs));
        }

        return ++action_it;
    }

    Action::ItrType RvmInsts::divwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using XLEN = uint64_t;
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        sreg_t lhs = sext32(rs1_val);
        sreg_t rhs = sext32(rs2_val);
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), UINT64_MAX);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext32(lhs / rhs));
        }

        return ++action_it;
    }

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RvmInsts::mulHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        WRITE_INT_REG<XLEN>(state, inst->getRd(), OPERATOR()(rs1_val, rs2_val));

        return ++action_it;
    }

    Action::ItrType RvmInsts::mulwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using XLEN = uint64_t;
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), sext32(rs1_val * rs2_val));

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvmInsts::remHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        sreg_t lhs = sext(rs1_val, state->getXlen());
        sreg_t rhs = sext(rs2_val, state->getXlen());

        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), lhs);
        }
        else if (lhs == INT64_MIN && rhs == -1)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), 0);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext((lhs % rhs), state->getXlen()));
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvmInsts::remuHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        reg_t lhs = zext(rs1_val, state->getXlen());
        reg_t rhs = zext(rs2_val, state->getXlen());
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext(lhs, state->getXlen()));
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext((lhs % rhs), state->getXlen()));
        }

        return ++action_it;
    }

    Action::ItrType RvmInsts::remuwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using XLEN = uint64_t;
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        reg_t lhs = zext32(rs1_val);
        reg_t rhs = zext32(rs2_val);
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext32(lhs));
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext32(lhs % rhs));
        }

        return ++action_it;
    }

    Action::ItrType RvmInsts::remwHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using XLEN = uint64_t;
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        sreg_t lhs = sext32(rs1_val);
        sreg_t rhs = sext32(rs2_val);
        if (rhs == 0)
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), lhs);
        }
        else
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), sext32(lhs % rhs));
        }

        return ++action_it;
    }
} // namespace pegasus
