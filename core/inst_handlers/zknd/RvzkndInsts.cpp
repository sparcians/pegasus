#include <cstdint>
#include "core/inst_handlers/zknd/RvzkndInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/Trap.hpp"
#include "core/inst_handlers/zknd/RvzkndFunctors.hpp"
#include "include/ActionTags.hpp"
#include "core/PegasusInst.hpp"

namespace pegasus
{

    template <typename XLEN>
    void RvzkndInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        if constexpr (sizeof(XLEN) == 4)
        {
            inst_handlers.emplace(
                "aes32dsi",
                Action::createAction<&RvzkndInsts::aesRHandler_<XLEN, Aes32DsiOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes32dsi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes32dsmi",
                Action::createAction<&RvzkndInsts::aesRHandler_<XLEN, Aes32DsmiOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes32dsmi", ActionTags::EXECUTE_TAG));
        }
        else
        {
            inst_handlers.emplace(
                "aes64ds",
                Action::createAction<&RvzkndInsts::aesRHandler_<XLEN, Aes64DsOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes64ds", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes64dsm",
                Action::createAction<&RvzkndInsts::aesRHandler_<XLEN, Aes64DsmOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes64dsm", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes64im",
                Action::createAction<&RvzkndInsts::aesIHandler_<XLEN, Aes64ImOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes64im", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes64ks1i",
                Action::createAction<&RvzkndInsts::aesKsiOpHandler_<XLEN, Aes64Ks1iOp<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes64ks1i", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes64ks2",
                Action::createAction<&RvzkndInsts::aesRHandler_<XLEN, Aes64Ks2Op<XLEN>>,
                                     RvzkndInsts>(nullptr, "aes64ks2", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzkndInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzkndInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RvzkndInsts::aesRHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const auto & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        const uint32_t imm = inst->getImmediate();
        const XLEN rd_val = OPERATOR{}(rs1_val, rs2_val, imm);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        return ++action_it;
    }

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RvzkndInsts::aesIHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const auto & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const uint32_t imm = inst->getImmediate();
        const XLEN rd_val = OPERATOR{}(rs1_val, 0, imm);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        return ++action_it;
    }

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RvzkndInsts::aesKsiOpHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const auto & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const uint32_t imm = inst->getImmediate();
        const uint32_t rnum = imm & 0xF;
        if (rnum > 10)
        {
            THROW_ILLEGAL_INST;
        }
        const XLEN rd_val = OPERATOR{}(rs1_val, 0, imm);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        return ++action_it;
    }

    template Action::ItrType RvzkndInsts::aesRHandler_<RV32, Aes32DsiOp<RV32>>(PegasusState*,
                                                                               Action::ItrType);

    template Action::ItrType RvzkndInsts::aesRHandler_<RV32, Aes32DsmiOp<RV32>>(PegasusState*,
                                                                                Action::ItrType);

    template Action::ItrType RvzkndInsts::aesRHandler_<RV64, Aes64DsOp<RV64>>(PegasusState*,
                                                                              Action::ItrType);

    template Action::ItrType RvzkndInsts::aesRHandler_<RV64, Aes64DsmOp<RV64>>(PegasusState*,
                                                                               Action::ItrType);

    template Action::ItrType RvzkndInsts::aesIHandler_<RV64, Aes64ImOp<RV64>>(PegasusState*,
                                                                              Action::ItrType);

    template Action::ItrType
    RvzkndInsts::aesKsiOpHandler_<RV64, Aes64Ks1iOp<RV64>>(PegasusState*, Action::ItrType);

    template Action::ItrType RvzkndInsts::aesRHandler_<RV64, Aes64Ks2Op<RV64>>(PegasusState*,
                                                                               Action::ItrType);

} // namespace pegasus