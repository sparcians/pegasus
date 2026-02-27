#include <limits>

#include "core/inst_handlers/v/RvvZvbbInsts.hpp"
#include "core/inst_handlers/i/RviFunctors.hpp"
#include "core/inst_handlers/b/RvbFunctors.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvZvbbInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vandn.vv",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::V},
                                                false, Andn>,
                RvvIntegerInsts>(nullptr, "vandn.vv", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vandn.vx",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::X},
                                                false, Andn>,
                RvvIntegerInsts>(nullptr, "vandn.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vbrev.v",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viUnaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::N},
                                                false, Brev>,
                RvvIntegerInsts>(nullptr, "vbrev.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vbrev8.v",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viUnaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::N},
                                                false, Brev8>,
                RvvIntegerInsts>(nullptr, "vbrev8.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vrev8.v",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viUnaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::N},
                                                false, ByteSwap>,
                RvvIntegerInsts>(nullptr, "vrev8.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vclz.v",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viUnaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::N},
                                                false, CountlZero>,
                RvvIntegerInsts>(nullptr, "vclz.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vctz.v",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viUnaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::N},
                                                false, CountrZero>,
                RvvIntegerInsts>(nullptr, "vctz.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vcpop.v",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viUnaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::N},
                                                false, Popcount>,
                RvvIntegerInsts>(nullptr, "vcpop.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vrol.vv",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::V},
                                                false, Rol>,
                RvvIntegerInsts>(nullptr, "vrol.vv", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vrol.vx",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::X},
                                                false, Rol>,
                RvvIntegerInsts>(nullptr, "vrol.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vror.vv",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::V},
                                                false, Ror>,
                RvvIntegerInsts>(nullptr, "vror.vv", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vror.vx",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::X},
                                                false, Ror>,
                RvvIntegerInsts>(nullptr, "vror.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vror.vi",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viBinaryHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::I},
                                                false, Ror>,
                RvvIntegerInsts>(nullptr, "vror.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwsll.vv",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viwsllHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::V}>,
                RvvZvbbInsts>(nullptr, "vwsll.vv", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwsll.vx",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viwsllHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::X}>,
                RvvZvbbInsts>(nullptr, "vwsll.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwsll.vi",
            pegasus::Action::createAction<
                &RvvZvbbInsts::viwsllHandler_<XLEN,
                                                OperandMode{.dst = OperandMode::Mode::V,
                                                            .src2 = OperandMode::Mode::V,
                                                            .src1 = OperandMode::Mode::I}>,
                RvvZvbbInsts>(nullptr, "vwsll.vi", ActionTags::EXECUTE_TAG));
    }

    template void RvvZvbbInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvZvbbInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, OperandMode opMode>
    Action::ItrType viwsllHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto elems_vs1 =
            opMode.src1 != OperandMode::Mode::V
                ? Elements<Element<elemWidth>, false>{}
                : Elements<Element<elemWidth>, false>{state, inst->getVecConfig(), inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, inst->getVecConfig(), inst->getRs2()};
        Elements<Element<2 * elemWidth>, false> elems_vd{state, inst->getVecConfig(),
                                                         inst->getRd()};
        const XLEN mask = (2 * elemWidth == 64) ? (1 << 6) - 1 :
                          (2 * elemWidth == 32) ? (1 << 5) - 1 :
                          (2 * elemWidth == 16) ? (1 << 4) - 1 :
                          0;
                          

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    elems_vd.getElement(index).setVal(
                        zext<UintType<2 * elemWidth>>(elems_vs2.getElement(index).getVal())
                        << (elems_vs1.getElement(index).getVal() & mask));
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(index).setVal(
                        zext<UintType<2 * elemWidth>>(elems_vs2.getElement(index).getVal())
                        << (READ_INT_REG<XLEN>(state, inst->getRs1()) & mask));
                }
                else // opMode.src1 == OperandMode::Mode::I)
                {
                    elems_vd.getElement(index).setVal(
                        zext<UintType<2 * elemWidth>>(elems_vs2.getElement(index).getVal())
                        << (inst->getImmediate() & mask));
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, inst->getVecConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode>
    Action::ItrType RvvZvbbInsts::viwsllHandler_(PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVecConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return viwsllHelper<XLEN, 8, opMode>(state, action_it);

            case 16:
                return viwsllHelper<XLEN, 16, opMode>(state, action_it);

            case 32:
                return viwsllHelper<XLEN, 32, opMode>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

} // namespace pegasus
