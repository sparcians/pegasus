#include "core/inst_handlers/zvbc/RvvZvbcInsts.hpp"
#include "core/inst_handlers/zvbc/RvvZvbcFunctors.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvZvbcInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vclmul.vv",
            pegasus::Action::createAction<
                &RvvZvbcInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::V},
                                                false, CarrylessMultiply>,
                RvvIntegerInsts>(nullptr, "vclmul.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vclmul.vx",
            pegasus::Action::createAction<
                &RvvZvbcInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::X},
                                                false, CarrylessMultiply>,
                RvvIntegerInsts>(nullptr, "vclmul.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vclmulh.vv",
            pegasus::Action::createAction<
                &RvvZvbcInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::V},
                                                false, CarrylessMultiplyHigh>,
                RvvIntegerInsts>(nullptr, "vclmulh.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vclmulh.vx",
            pegasus::Action::createAction<
                &RvvZvbcInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::X},
                                                false, CarrylessMultiplyHigh>,
                RvvIntegerInsts>(nullptr, "vclmulh.vx", ActionTags::EXECUTE_TAG));
    }

    template void RvvZvbcInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvvZvbcInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);
} // namespace pegasus
