#include "core/inst_handlers/v/RvvPermuteInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvPermuteInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vmv.s.x",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::X}>,
                RvvPermuteInsts>(nullptr, "vmv.s.x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv.x.s",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::X,
                                                                .src2 = OperandMode::Mode::V}>,
                RvvPermuteInsts>(nullptr, "vmv.x.s", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfmv.s.f",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::V,
                                                                .src1 = OperandMode::Mode::F}>,
                RvvPermuteInsts>(nullptr, "vfmv.s.x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmv.f.s",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::F,
                                                                .src2 = OperandMode::Mode::V}>,
                RvvPermuteInsts>(nullptr, "vfmv.x.s", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vslideup.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslideHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::X},
                                                 true>,
                RvvPermuteInsts>(nullptr, "vslideup.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vslideup.vi",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslideHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::I},
                                                 true>,
                RvvPermuteInsts>(nullptr, "vslideup.vi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vslidedown.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslideHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::X},
                                                 false>,
                RvvPermuteInsts>(nullptr, "vslidedown.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vslidedown.vi",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslideHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::I},
                                                 false>,
                RvvPermuteInsts>(nullptr, "vslidedown.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vslide1up.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslide1Handler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::X},
                                                  true>,
                RvvPermuteInsts>(nullptr, "vslide1up.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfslide1up.vf",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslide1Handler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::F},
                                                  true>,
                RvvPermuteInsts>(nullptr, "vfslide1up.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vslide1down.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslide1Handler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::X},
                                                  false>,
                RvvPermuteInsts>(nullptr, "vslide1down.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfslide1down.vf",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslide1Handler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::F},
                                                  false>,
                RvvPermuteInsts>(nullptr, "vfslide1down.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vrgather.vv",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vrgatherHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false>,
                RvvPermuteInsts>(nullptr, "vrgather.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vrgather.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vrgatherHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false>,
                RvvPermuteInsts>(nullptr, "vrgather.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vrgather.vi",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vrgatherHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::I},
                                                   false>,
                RvvPermuteInsts>(nullptr, "vrgather.vi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vrgatherei16.vv",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vrgatherHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   true>,
                RvvPermuteInsts>(nullptr, "vrgatherei16.vv", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vcompress.vm",
            pegasus::Action::createAction<&RvvPermuteInsts::vcompressHandler_, RvvPermuteInsts>(
                nullptr, "vcompress.vm", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmv1r.v",
            pegasus::Action::createAction<&RvvPermuteInsts::vmvrHandler_<1>, RvvPermuteInsts>(
                nullptr, "vmv1r.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv2r.v",
            pegasus::Action::createAction<&RvvPermuteInsts::vmvrHandler_<2>, RvvPermuteInsts>(
                nullptr, "vmv2r.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv4r.v",
            pegasus::Action::createAction<&RvvPermuteInsts::vmvrHandler_<4>, RvvPermuteInsts>(
                nullptr, "vmv4r.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv8r.v",
            pegasus::Action::createAction<&RvvPermuteInsts::vmvrHandler_<8>, RvvPermuteInsts>(
                nullptr, "vmv8r.v", ActionTags::EXECUTE_TAG));
    }

    template void RvvPermuteInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvPermuteInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, OperandMode opMode>
    Action::ItrType vmvHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        VectorConfig* vector_config = state->getVectorConfig();

        if constexpr ((opMode.dst == OperandMode::Mode::F || opMode.dst == OperandMode::Mode::X)
                      && opMode.src2 == OperandMode::Mode::V)
        {
            Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                          inst->getRs2()};
            if constexpr (opMode.dst == OperandMode::Mode::X)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    sext<XLEN>(elems_vs2.getElement(0).getVal()));
            }
            else
            {
                WRITE_FP_REG<RV64>(state, inst->getRd(), elems_vs2.getElement(0).getVal());
            }
        }
        else if constexpr ((opMode.src1 == OperandMode::Mode::F
                            || opMode.src1 == OperandMode::Mode::X)
                           && opMode.dst == OperandMode::Mode::V)
        {
            if (vector_config->getVSTART() < vector_config->getVL())
            {
                using ValueType = typename Element<elemWidth>::ValueType;
                Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                             inst->getRd()};
                if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(0).setVal(
                        sext<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else
                {
                    elems_vd.getElement(0).setVal(READ_FP_REG<RV64>(state, inst->getRs1()));
                }
            }
        }
        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode>
    Action::ItrType RvvPermuteInsts::vmvHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                if constexpr (opMode.dst == OperandMode::Mode::F
                              || opMode.src1 == OperandMode::Mode::F)
                {
                    break;
                }
                return vmvHelper<XLEN, 8, opMode>(state, action_it);

            case 16:
                return vmvHelper<XLEN, 16, opMode>(state, action_it);

            case 32:
                return vmvHelper<XLEN, 32, opMode>(state, action_it);

            case 64:
                return vmvHelper<XLEN, 64, opMode>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool isUp>
    Action::ItrType vslideHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const size_t offset = opMode.src1 == OperandMode::Mode::I
                                  ? inst->getImmediate()
                                  : READ_INT_REG<XLEN>(state, inst->getRs1());
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (isUp)
                {
                    elems_vd.getElement(index).setVal(
                        elems_vs2.getElement(index - offset).getVal());
                }
                else
                {
                    using ValueType = typename Element<elemWidth>::ValueType;
                    ValueType val = (index + offset) < state->getVectorConfig()->getVLMAX()
                                        ? elems_vs2.getElement(index + offset).getVal()
                                        : 0;
                    elems_vd.getElement(index).setVal(val);
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            auto begin = isUp ? typename decltype(elems_vd)::ElementIterator(&elems_vd, offset)
                              : elems_vd.begin();
            execute(begin, elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            auto begin =
                isUp ? MaskBitIterator(&mask_elems, offset) : mask_elems.maskBitIterBegin();
            execute(begin, mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isUp>
    Action::ItrType RvvPermuteInsts::vslideHandler_(pegasus::PegasusState* state,
                                                    Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vslideHelper<XLEN, 8, opMode, isUp>(state, action_it);

            case 16:
                return vslideHelper<XLEN, 16, opMode, isUp>(state, action_it);

            case 32:
                return vslideHelper<XLEN, 32, opMode, isUp>(state, action_it);

            case 64:
                return vslideHelper<XLEN, 64, opMode, isUp>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool isUp>
    Action::ItrType vslide1Helper(PegasusState* state, Action::ItrType action_it)
    {
        using ValueType = typename Element<elemWidth>::ValueType;
        const PegasusInstPtr & inst = state->getCurrentInst();
        const size_t offset = 1;
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};

        auto execute = [&](auto iter, const auto & end)
        {
            const auto vl = state->getVectorConfig()->getVL();
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (isUp)
                {
                    elems_vd.getElement(index).setVal(
                        elems_vs2.getElement(index - offset).getVal());
                }
                else
                {
                    ValueType val = (index + offset) < state->getVectorConfig()->getVLMAX()
                                        ? elems_vs2.getElement(index + offset).getVal()
                                        : 0;
                    elems_vd.getElement(index).setVal(val);
                }
            }
            if (!isUp && index == vl - 1)
            {
                if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(index).setVal(
                        sext<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else
                {
                    elems_vd.getElement(index).setVal(READ_FP_REG<RV64>(state, inst->getRs1()));
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            if (isUp && state->getVectorConfig()->getVSTART() == 0)
            {
                if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(0).setVal(
                        sext<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else
                {
                    elems_vd.getElement(0).setVal(READ_FP_REG<RV64>(state, inst->getRs1()));
                }
            }
            auto begin = isUp ? typename decltype(elems_vd)::ElementIterator(&elems_vd, offset)
                              : elems_vd.begin();
            execute(begin, elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            if (isUp && state->getVectorConfig()->getVSTART() == 0 && mask_elems.getBit(0))
            {
                if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(0).setVal(
                        sext<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else
                {
                    elems_vd.getElement(0).setVal(READ_FP_REG<RV64>(state, inst->getRs1()));
                }
            }
            auto begin =
                isUp ? MaskBitIterator(&mask_elems, offset) : mask_elems.maskBitIterBegin();
            execute(begin, mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isUp>
    Action::ItrType RvvPermuteInsts::vslide1Handler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vslide1Helper<XLEN, 8, opMode, isUp>(state, action_it);

            case 16:
                return vslide1Helper<XLEN, 16, opMode, isUp>(state, action_it);

            case 32:
                return vslide1Helper<XLEN, 32, opMode, isUp>(state, action_it);

            case 64:
                return vslide1Helper<XLEN, 64, opMode, isUp>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool is16>
    Action::ItrType vrgatherHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<is16 ? 16 : elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                                  inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};
        size_t i = opMode.src1 == OperandMode::Mode::I   ? inst->getImmediate()
                   : opMode.src1 == OperandMode::Mode::X ? READ_INT_REG<XLEN>(state, inst->getRs1())
                                                         : 0;

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if (opMode.src1 == OperandMode::Mode::V)
                {
                    i = elems_vs1.getElement(index).getVal();
                }
                if (i >= state->getVectorConfig()->getVLMAX())
                {
                    elems_vd.getElement(index).setVal(0);
                }
                else
                {
                    elems_vd.getElement(index).setVal(elems_vs2.getElement(i).getVal());
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

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool is16>
    Action::ItrType RvvPermuteInsts::vrgatherHandler_(pegasus::PegasusState* state,
                                                      Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vrgatherHelper<XLEN, 8, opMode, is16>(state, action_it);

            case 16:
                return vrgatherHelper<XLEN, 16, opMode, is16>(state, action_it);

            case 32:
                return vrgatherHelper<XLEN, 32, opMode, is16>(state, action_it);

            case 64:
                return vrgatherHelper<XLEN, 64, opMode, is16>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <size_t elemWidth>
    Action::ItrType vcompressHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};
        const MaskElements mask_elems{state, state->getVectorConfig(), inst->getRs1()};
        size_t i = 0;

        for (auto iter = mask_elems.maskBitIterBegin(); iter != mask_elems.maskBitIterEnd();
             ++iter, ++i)
        {
            elems_vd.getElement(i).setVal(elems_vs2.getElement(iter.getIndex()).getVal());
        }

        return ++action_it;
    }

    Action::ItrType RvvPermuteInsts::vcompressHandler_(pegasus::PegasusState* state,
                                                       Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vcompressHelper<8>(state, action_it);

            case 16:
                return vcompressHelper<16>(state, action_it);

            case 32:
                return vcompressHelper<32>(state, action_it);

            case 64:
                return vcompressHelper<64>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <size_t elemWidth, size_t nReg>
    Action::ItrType vmvrHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        VectorConfig config{*state->getVectorConfig()};
        config.setLMUL(nReg * 8);
        config.setVL(config.getVLMAX());
        Elements<Element<elemWidth>, false> elems_vs2{state, &config, inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, &config, inst->getRd()};

        for (auto iter = elems_vd.begin(); iter != elems_vd.end(); ++iter)
        {
            auto index = iter.getIndex();
            elems_vd.getElement(index).setVal(elems_vs2.getElement(index).getVal());
        }

        return ++action_it;
    }

    template <size_t nReg>
    Action::ItrType RvvPermuteInsts::vmvrHandler_(pegasus::PegasusState* state,
                                                  Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vmvrHelper<8, nReg>(state, action_it);

            case 16:
                return vmvrHelper<16, nReg>(state, action_it);

            case 32:
                return vmvrHelper<32, nReg>(state, action_it);

            case 64:
                return vmvrHelper<64, nReg>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }
} // namespace pegasus
