#include "core/inst_handlers/zfbfmin/RvzfbfminInsts.hpp"

#include "core/inst_handlers/i/RviInsts.hpp"

#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzfbfminInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        (void)inst_handlers;
        inst_handlers.emplace(
            "fcvt.bf16.s", pegasus::Action::createAction<&RvzfbfminInsts::fcvt_bf16_sHandler_<XLEN>,
                                                         RvzfbfminInsts>(nullptr, "fcvt_bf16_s",
                                                                         ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "fcvt.s.bf16", pegasus::Action::createAction<&RvzfbfminInsts::fcvt_s_bf16Handler<XLEN>,
                                                         RvzfbfminInsts>(nullptr, "fcvt_s_bf16",
                                                                         ActionTags::EXECUTE_TAG));
    }

    template void RvzfbfminInsts::getInstHandlers<RV32>(InstHandlers::InstHandlersMap &);
    template void RvzfbfminInsts::getInstHandlers<RV64>(InstHandlers::InstHandlersMap &);

    template <typename XLEN>
    Action::ItrType RvzfbfminInsts::fcvt_bf16_sHandler_(pegasus::PegasusState* state,
                                                        Action::ItrType action_it)
    {
        // Convert FP32 value to a BF16 value
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_SP rs1_val =
            checkNanBoxing<RV64, FLOAT_SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        softfloat_roundingMode = getRM<XLEN>(state);
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, FLOAT_BF16>(f32_to_bf16(float32_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzfbfminInsts::fcvt_s_bf16Handler(pegasus::PegasusState* state,
                                                       Action::ItrType action_it)
    {
        // Convert BF16 value to a FP32 value
        const PegasusInstPtr & inst = state->getCurrentInst();
        const FLOAT_BF16 rs1_val =
            checkNanBoxing<RV64, FLOAT_BF16>(READ_FP_REG<RV64>(state, inst->getRs1()));
        softfloat_roundingMode = getRM<XLEN>(state);
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, FLOAT_SP>(bf16_to_f32(bfloat16_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

} // namespace pegasus
