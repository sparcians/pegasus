#include <limits>

#include "core/inst_handlers/v/RvvFixedPointInsts.hpp"
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
                                                      false, SatAdd>,
                RvvFixedPointInsts>(nullptr, "vsaddu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsaddu.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      false, SatAdd>,
                RvvFixedPointInsts>(nullptr, "vsaddu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsaddu.vi",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::I},
                                                      false, SatAdd>,
                RvvFixedPointInsts>(nullptr, "vsaddu.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsadd.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      true, SatAdd>,
                RvvFixedPointInsts>(nullptr, "vsadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsadd.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      true, SatAdd>,
                RvvFixedPointInsts>(nullptr, "vsadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsadd.vi",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::I},
                                                      true, SatAdd>,
                RvvFixedPointInsts>(nullptr, "vsadd.vi", ActionTags::EXECUTE_TAG));

        // Saturating Sub
        inst_handlers.emplace(
            "vssubu.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      false, SatSub>,
                RvvFixedPointInsts>(nullptr, "vssubu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vssubu.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      false, SatSub>,
                RvvFixedPointInsts>(nullptr, "vssubu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vssub.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      true, SatSub>,
                RvvFixedPointInsts>(nullptr, "vssub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vssub.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      true, SatSub>,
                RvvFixedPointInsts>(nullptr, "vssub.vx", ActionTags::EXECUTE_TAG));

        // Averaging Add
        inst_handlers.emplace(
            "vaaddu.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      false, AveAdd>,
                RvvFixedPointInsts>(nullptr, "vaaddu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vaaddu.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      false, AveAdd>,
                RvvFixedPointInsts>(nullptr, "vaaddu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vaadd.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      true, AveAdd>,
                RvvFixedPointInsts>(nullptr, "vaadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vaadd.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      true, AveAdd>,
                RvvFixedPointInsts>(nullptr, "vaadd.vx", ActionTags::EXECUTE_TAG));

        // Averaging Sub
        inst_handlers.emplace(
            "vasubu.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      false, AveSub>,
                RvvFixedPointInsts>(nullptr, "vasubu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vasubu.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      false, AveSub>,
                RvvFixedPointInsts>(nullptr, "vasubu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vasub.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      true, AveSub>,
                RvvFixedPointInsts>(nullptr, "vasub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vasub.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      true, AveSub>,
                RvvFixedPointInsts>(nullptr, "vasub.vx", ActionTags::EXECUTE_TAG));

        // Saturating Mul
        inst_handlers.emplace(
            "vsmul.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      true, SatMul>,
                RvvFixedPointInsts>(nullptr, "vsmul.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsmul.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      true, SatMul>,
                RvvFixedPointInsts>(nullptr, "vsmul.vx", ActionTags::EXECUTE_TAG));

        // Scaling shift right logical
        inst_handlers.emplace(
            "vssrl.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      false, SclSrl>,
                RvvFixedPointInsts>(nullptr, "vssrl.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vssrl.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      false, SclSrl>,
                RvvFixedPointInsts>(nullptr, "vssrl.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vssrl.vi",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::I},
                                                      false, SclSrl>,
                RvvFixedPointInsts>(nullptr, "vssrl.vi", ActionTags::EXECUTE_TAG));

        // Scaling shift right arithmetic
        inst_handlers.emplace(
            "vssra.vv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::V},
                                                      false, SclSra>,
                RvvFixedPointInsts>(nullptr, "vssra.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vssra.vx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::X},
                                                      false, SclSra>,
                RvvFixedPointInsts>(nullptr, "vssra.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vssra.vi",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::V,
                                                                  .src1 = OperandMode::Mode::I},
                                                      false, SclSra>,
                RvvFixedPointInsts>(nullptr, "vssra.vi", ActionTags::EXECUTE_TAG));

        // Narrowing Clip
        inst_handlers.emplace(
            "vnclipu.wv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::W,
                                                                  .src1 = OperandMode::Mode::V},
                                                      false, NClip>,
                RvvFixedPointInsts>(nullptr, "vnclipu.wv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnclipu.wx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::W,
                                                                  .src1 = OperandMode::Mode::X},
                                                      false, NClip>,
                RvvFixedPointInsts>(nullptr, "vnclipu.wx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnclipu.wi",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxBinaryHandler_<XLEN,
                                                      OperandMode{.dst = OperandMode::Mode::V,
                                                                  .src2 = OperandMode::Mode::W,
                                                                  .src1 = OperandMode::Mode::I},
                                                      false, NClip>,
                RvvFixedPointInsts>(nullptr, "vnclipu.wi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vnclip.wv",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxNClipHandler_<XLEN,
                                                     OperandMode{.dst = OperandMode::Mode::V,
                                                                 .src2 = OperandMode::Mode::W,
                                                                 .src1 = OperandMode::Mode::V},
                                                     true, NClip>,
                RvvFixedPointInsts>(nullptr, "vnclip.wv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnclip.wx",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxNClipHandler_<XLEN,
                                                     OperandMode{.dst = OperandMode::Mode::V,
                                                                 .src2 = OperandMode::Mode::W,
                                                                 .src1 = OperandMode::Mode::X},
                                                     true, NClip>,
                RvvFixedPointInsts>(nullptr, "vnclip.wx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vnclip.wi",
            pegasus::Action::createAction<
                &RvvFixedPointInsts::vxNClipHandler_<XLEN,
                                                     OperandMode{.dst = OperandMode::Mode::V,
                                                                 .src2 = OperandMode::Mode::W,
                                                                 .src1 = OperandMode::Mode::I},
                                                     true, NClip>,
                RvvFixedPointInsts>(nullptr, "vnclip.wi", ActionTags::EXECUTE_TAG));
    }

    template void RvvFixedPointInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvFixedPointInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, OperandMode opMode, typename Functor>
    Action::ItrType vxBinaryHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto elems_vs2 =
            Elements<Element<opMode.src2 == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>,
                     false>{state, state->getVectorConfig(), inst->getRs1()};
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, state->getVectorConfig(),
                                                                   inst->getRs1()};
        auto elems_vd =
            Elements<Element<elemWidth>, false>{state, state->getVectorConfig(), inst->getRd()};
        using S = typename decltype(elems_vs2)::ElemType::ValueType;
        using R = typename decltype(elems_vd)::ElemType::ValueType;
        Functor functor{};
        sat = READ_CSR_FIELD<XLEN>(state, VXSAT, "VXSAT");
        xrm = static_cast<Xrm>(READ_CSR_FIELD<XLEN>(state, VXRM, "VXRM"));

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    elems_vd.getElement(index).setVal(
                        static_cast<R>(functor(elems_vs2.getElement(index).getVal(),
                                               elems_vs1.getElement(index).getVal())));
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(index).setVal(static_cast<R>(
                        functor(elems_vs2.getElement(index).getVal(),
                                static_cast<S>(READ_INT_REG<XLEN>(state, inst->getRs1())))));
                }
                else // opMode.src1 == OperandMode::Mode::I
                {
                    elems_vd.getElement(index).setVal(
                        static_cast<R>(functor(elems_vs2.getElement(index).getVal(),
                                               static_cast<S>(inst->getImmediate()))));
                }
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
        WRITE_CSR_FIELD<XLEN>(state, VXSAT, "VXSAT", sat);
        WRITE_CSR_FIELD<XLEN>(state, VCSR, "VXSAT", sat);

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
                if constexpr (opMode.src2 == OperandMode::Mode::W)
                {
                    sparta_assert(false, "Unsupported SEW value");
                }
                else
                {
                    return vxBinaryHelper<
                        XLEN, 64, opMode,
                        FunctorTemp<std::conditional_t<isSigned, int64_t, uint64_t>>>(state,
                                                                                      action_it);
                }
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isSigned,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvFixedPointInsts::vxNClipHandler_(PegasusState* state,
                                                        Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();

        switch (vector_config->getSEW())
        {
            case 8:
                return vxBinaryHelper<XLEN, 8, opMode,
                                      FunctorTemp<std::conditional_t<isSigned, int16_t, uint16_t>>>(
                    state, action_it);
            case 16:
                return vxBinaryHelper<XLEN, 16, opMode,
                                      FunctorTemp<std::conditional_t<isSigned, int64_t, uint64_t>>>(
                    state, action_it);
            case 32:
                return vxBinaryHelper<XLEN, 32, opMode,
                                      FunctorTemp<std::conditional_t<isSigned, int64_t, uint64_t>>>(
                    state, action_it);
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }

        return ++action_it;
    }
} // namespace pegasus
