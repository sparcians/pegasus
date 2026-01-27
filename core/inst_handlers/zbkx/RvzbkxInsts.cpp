#include "core/inst_handlers/zbkx/RvzbkxInsts.hpp"
#include "core/PegasusState.hpp"
#include "include/ActionTags.hpp"
#include "include/IntNums.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzbkxInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {

        inst_handlers.emplace(
            "xperm8", Action::createAction<&RvzbkxInsts::xpermHandler_<XLEN, Xperm8Op>, RvzbkxInsts>(
                          nullptr, "xperm8", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "xperm4", Action::createAction<&RvzbkxInsts::xpermHandler_<XLEN, Xperm4Op>, RvzbkxInsts>(
                          nullptr, "xperm4", ActionTags::EXECUTE_TAG));
    }

    template void RvzbkxInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzbkxInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RvzbkxInsts::xpermHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const auto & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = OPERATOR()(rs1_val, rs2_val);

        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template Action::ItrType RvzbkxInsts::xpermHandler_<RV32, RvzbkxInsts::Xperm8Op>(PegasusState*, Action::ItrType);
    template Action::ItrType RvzbkxInsts::xpermHandler_<RV64, RvzbkxInsts::Xperm8Op>(PegasusState*, Action::ItrType);
    template Action::ItrType RvzbkxInsts::xpermHandler_<RV32, RvzbkxInsts::Xperm4Op>(PegasusState*, Action::ItrType);
    template Action::ItrType RvzbkxInsts::xpermHandler_<RV64, RvzbkxInsts::Xperm4Op>(PegasusState*, Action::ItrType);

    template <typename XLEN> XLEN RvzbkxInsts::Xperm8Op::operator()(XLEN rs1, XLEN rs2) const
    {
        XLEN result = 0;
        constexpr size_t num_bytes = sizeof(XLEN);
        for (size_t i = 0; i < num_bytes; ++i)
        {
            uint8_t idx = (rs2 >> (i * 8)) & 0xFF;
            if (idx < num_bytes)
            {
                uint8_t byte = (rs1 >> (idx * 8)) & 0xFF;
                result |= (static_cast<XLEN>(byte) << (i * 8));
            }
        }
        return result;
    }

    template RV32 RvzbkxInsts::Xperm8Op::operator()<RV32>(RV32, RV32) const;
    template RV64 RvzbkxInsts::Xperm8Op::operator()<RV64>(RV64, RV64) const;

    template <typename XLEN> XLEN RvzbkxInsts::Xperm4Op::operator()(XLEN rs1, XLEN rs2) const
    {
        XLEN result = 0;
        constexpr size_t num_nibbles = sizeof(XLEN) * 2;
        for (size_t i = 0; i < num_nibbles; ++i)
        {
            uint8_t idx = (rs2 >> (i * 4)) & 0xF;
            if (idx < num_nibbles)
            {
                uint8_t nibble = (rs1 >> (idx * 4)) & 0xF;
                result |= (static_cast<XLEN>(nibble) << (i * 4));
            }
        }
        return result;
    }

    template RV32 RvzbkxInsts::Xperm4Op::operator()<RV32>(RV32, RV32) const;
    template RV64 RvzbkxInsts::Xperm4Op::operator()<RV64>(RV64,  RV64) const;
}