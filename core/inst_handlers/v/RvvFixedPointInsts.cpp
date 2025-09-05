#include <limits>

#include "core/inst_handlers/v/RvvFixedPointInsts.hpp"
#include "core/inst_handlers/finsts_helpers.hpp"
#include "core/inst_handlers/v/RvvFunctors.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvFixedPointInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // Saturating Add
        inst_handlers.emplace(
            "vsaddu.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      false, AddSat>,
                RvvFixedPointInsts>(nullptr, "vsaddu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsaddu.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      false, AddSat>,
                RvvFixedPointInsts>(nullptr, "vsaddu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsaddu.vi",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::I},
                                                      false, AddSat>,
                RvvFixedPointInsts>(nullptr, "vsaddu.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsadd.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      true, AddSat>,
                RvvFixedPointInsts>(nullptr, "vsadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsadd.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      true, AddSat>,
                RvvFixedPointInsts>(nullptr, "vsadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsadd.vi",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::I},
                                                      true, AddSat>,
                RvvFixedPointInsts>(nullptr, "vsadd.vi", ActionTags::EXECUTE_TAG));

        // Saturating Sub
        inst_handlers.emplace(
            "vssubu.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      false, SubSat>,
                RvvFixedPointInsts>(nullptr, "vssubu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vssubu.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      false, SubSat>,
                RvvFixedPointInsts>(nullptr, "vssubu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vssub.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      true, SubSat>,
                RvvFixedPointInsts>(nullptr, "vssub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vssub.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      true, SubSat>,
                RvvFixedPointInsts>(nullptr, "vssub.vx", ActionTags::EXECUTE_TAG));
    }

    template void RvvFixedPointInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvFixedPointInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, OperandMode opMode, typename Functor>
    Action::ItrType vxBinaryHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto elems_vs2 =
            Elements<Element<elemWidth>, false>{state, state->getVectorConfig(), inst->getRs1()};
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        auto elems_vd =
            Elements<Element<elemWidth>, false>{state, state->getVectorConfig(), inst->getRd()};
        bool saturation = false;
        Functor functor{};
        using ValueType = typename decltype(elems_vd)::ElemType::ValueType;

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                bool sat = false;
                index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    elems_vd.getElement(index).setVal(functor(elems_vs2.getElement(index).getVal(),
                                                              elems_vs1.getElement(index).getVal(),
                                                              sat));
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(index).setVal(functor(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())), sat));
                }
                else // opMode.src1 == OperandMode::Mode::I
                {
                    elems_vd.getElement(index).setVal(
                        functor(elems_vs2.getElement(index).getVal(),
                                static_cast<ValueType>(inst->getImmediate()), sat));
                }
                saturation |= sat;
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }
        updateFixedCsrs<XLEN>(state, saturation);

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isSigned,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvFixedPointInsts::vxBinaryHandler_(PegasusState* state,
                                                         Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vxBinaryHelper<XLEN, 8, opMode,
                                      FunctorTemp<std::conditional_t<isSigned, int8_t, uint8_t>>>(
                    state, action_it);

            case 16:
                return vxBinaryHelper<XLEN, 16, opMode,
                                      FunctorTemp<std::conditional_t<isSigned, int16_t, uint16_t>>>(
                    state, action_it);

            case 32:
                return vxBinaryHelper<XLEN, 32, opMode,
                                      FunctorTemp<std::conditional_t<isSigned, int32_t, uint32_t>>>(
                    state, action_it);

            case 64:
                return vxBinaryHelper<XLEN, 64, opMode,
                                      FunctorTemp<std::conditional_t<isSigned, int64_t, uint64_t>>>(
                    state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }

        return ++action_it;
    }
} // namespace pegasus
