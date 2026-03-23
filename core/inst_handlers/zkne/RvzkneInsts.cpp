#include "core/PegasusState.hpp"
#include "core/inst_handlers/zkne/RvzkneInsts.hpp"
#include "core/InstHandlers.hpp"
#include "RvzkneFunctors.hpp"
#include "core/inst_handlers/zknd/RvzkndFunctors.hpp" // for the last two functors

namespace pegasus
{
    template <typename XLEN>
    void RvzkneInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        if constexpr (sizeof(XLEN) == 4)
        {
            // aes32esi and aes32esmi handlers

            inst_handlers.emplace(
                "aes32esi",
                Action::createAction<&RvzkneInsts::aesHandler_<XLEN, Aes32esHandler<XLEN>>,
                                     RvzkneInsts>(nullptr, "aes32esi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "aes32esmi",
                Action::createAction<&RvzkneInsts::aesHandler_<XLEN, Aes32esmHandler<XLEN>>,
                                     RvzkneInsts>(nullptr, "aes32esmi", ActionTags::EXECUTE_TAG));
        }
        else
        {
            // aes64es , aes64esm
            inst_handlers.emplace(
                "aes64es",
                Action::createAction<&RvzkneInsts::aesHandler_<XLEN, Aes64esHandler<XLEN>>,
                                     RvzkneInsts>(nullptr, "aes64es", ActionTags::EXECUTE_TAG));

            inst_handlers.emplace(
                "aes64esm",
                Action::createAction<&RvzkneInsts::aesHandler_<XLEN, Aes64esmHandler<XLEN>>,
                                     RvzkneInsts>(nullptr, "aes64esm", ActionTags::EXECUTE_TAG));

            inst_handlers.emplace(
                "aes64ks1i",
                Action::createAction<&RvzkneInsts::aesHandler_<XLEN, Aes64Ks1iOp<XLEN>>,
                                     RvzkneInsts>(nullptr, "aes64ks1i", ActionTags::EXECUTE_TAG));

            inst_handlers.emplace(
                "aes64ks2",
                Action::createAction<&RvzkneInsts::aesHandler_<XLEN, Aes64Ks2Op<XLEN>>,
                                     RvzkneInsts>(nullptr, "aes64ks2", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzkneInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzkneInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RvzkneInsts::aesHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const auto & inst = state->getCurrentInst();
        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        const uint32_t imm = inst->getImmediate();
        const XLEN rd_val = OPERATOR{}(rs1_val, rs2_val, imm);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);
        return ++action_it;
    }

    template Action::ItrType RvzkneInsts::aesHandler_<RV32, Aes32esHandler<RV32>>(PegasusState*,
                                                                                  Action::ItrType);

    template Action::ItrType RvzkneInsts::aesHandler_<RV32, Aes32esmHandler<RV32>>(PegasusState*,
                                                                                   Action::ItrType);

    template Action::ItrType RvzkneInsts::aesHandler_<RV64, Aes64esHandler<RV64>>(PegasusState*,
                                                                                  Action::ItrType);

    template Action::ItrType RvzkneInsts::aesHandler_<RV64, Aes64esmHandler<RV64>>(PegasusState*,
                                                                                   Action::ItrType);

    template Action::ItrType RvzkneInsts::aesHandler_<RV64, Aes64Ks1iOp<RV64>>(PegasusState*, Action::ItrType);

    template Action::ItrType RvzkneInsts::aesHandler_<RV64, Aes64Ks2Op<RV64>>(PegasusState*, Action::ItrType);
} // namespace pegasus
