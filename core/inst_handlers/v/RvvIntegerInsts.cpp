#include "core/inst_handlers/v/RvvIntegerInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvvIntegerInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vadd.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::viaHandler_<XLEN, OperandMode::VV, std::plus>, RvvIntegerInsts>(
                nullptr, "vadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::viaHandler_<XLEN, OperandMode::VX, std::plus>, RvvIntegerInsts>(
                nullptr, "vadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vi",
            atlas::Action::createAction<
                &RvvIntegerInsts::viaHandler_<XLEN, OperandMode::VI, std::plus>, RvvIntegerInsts>(
                nullptr, "vadd.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsub.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::viaHandler_<XLEN, OperandMode::VV, std::minus>, RvvIntegerInsts>(
                nullptr, "vsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsub.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::viaHandler_<XLEN, OperandMode::VX, std::minus>, RvvIntegerInsts>(
                nullptr, "vsub.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwadd.vv", atlas::Action::createAction<
                            &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::VV, true, std::plus>,
                            RvvIntegerInsts>(nullptr, "vwadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwadd.vx", atlas::Action::createAction<
                            &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::VX, true, std::plus>,
                            RvvIntegerInsts>(nullptr, "vwadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwadd.wv", atlas::Action::createAction<
                            &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::WV, true, std::plus>,
                            RvvIntegerInsts>(nullptr, "vwadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwadd.wx", atlas::Action::createAction<
                            &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::WX, true, std::plus>,
                            RvvIntegerInsts>(nullptr, "vwadd.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwaddu.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::VV, false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwaddu.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::VX, false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwaddu.wv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::WV, false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwaddu.wx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::WX, false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwsub.vv", atlas::Action::createAction<
                            &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::VV, true, std::plus>,
                            RvvIntegerInsts>(nullptr, "vwsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsub.vx", atlas::Action::createAction<
                            &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::VX, true, std::plus>,
                            RvvIntegerInsts>(nullptr, "vwsub.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsub.wv", atlas::Action::createAction<
                            &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::WV, true, std::plus>,
                            RvvIntegerInsts>(nullptr, "vwsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsub.wx", atlas::Action::createAction<
                            &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::WX, true, std::plus>,
                            RvvIntegerInsts>(nullptr, "vwsub.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwsubu.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::VV, false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsubu.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::VX, false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsubu.wv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::WV, false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsubu.wx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vwiaHandler_<XLEN, OperandMode::WX, false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vx", ActionTags::EXECUTE_TAG));
    }

    template void RvvIntegerInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvIntegerInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t ElemWidth, RvvIntegerInsts::OperandMode opMode,
              typename Functor>
    Action::ItrType viaHelper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<ElemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<ElemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<ElemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            size_t index = 0;
            for (auto iter = begin; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode == RvvIntegerInsts::OperandMode::VV)
                {
                    elems_vd.getElement(index).setVal(
                        Functor()(elems_vs1.getElement(index).getVal(),
                                  elems_vs2.getElement(index).getVal()));
                }
                else if (opMode == RvvIntegerInsts::OperandMode::VX)
                {
                    elems_vd.getElement(index).setVal(
                        Functor()(static_cast<typename Functor::result_type>(
                                      READ_INT_REG<XLEN>(state, inst->getRs1())),
                                  elems_vs2.getElement(index).getVal()));
                }
                else // OperandMode::VI
                {
                    elems_vd.getElement(index).setVal(
                        Functor()(static_cast<typename Functor::result_type>(inst->getImmediate()),
                                  elems_vs2.getElement(index).getVal()));
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, RvvIntegerInsts::OperandMode opMode,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::viaHandler_(AtlasState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return viaHelper<XLEN, 8, opMode, FunctorTemp<uint8_t>>(state, action_it);
                break;
            case 16:
                return viaHelper<XLEN, 16, opMode, FunctorTemp<uint16_t>>(state, action_it);
                break;
            case 32:
                return viaHelper<XLEN, 32, opMode, FunctorTemp<uint32_t>>(state, action_it);
                break;
            case 64:
                return viaHelper<XLEN, 64, opMode, FunctorTemp<uint64_t>>(state, action_it);
                break;
            default:
                sparta_assert(false, "Invalid SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t ElemWidth, RvvIntegerInsts::OperandMode opMode,
              typename Functor>
    Action::ItrType vwiaHelper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<ElemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        constexpr size_t ElemWidthVs2 = (opMode == RvvIntegerInsts::OperandMode::WX
                                         || opMode == RvvIntegerInsts::OperandMode::WV)
                                            ? 2 * ElemWidth
                                            : ElemWidth;
        Elements<Element<ElemWidthVs2>, false> elems_vs2{state, state->getVectorConfig(),
                                                         inst->getRs2()};
        Elements<Element<2 * ElemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                         inst->getRd()};

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            size_t index = 0;
            for (auto iter = begin; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode == RvvIntegerInsts::OperandMode::VV
                              || opMode == RvvIntegerInsts::OperandMode::WV)
                {
                    elems_vd.getElement(index).setVal(
                        Functor()(elems_vs1.getElement(index).getVal(),
                                  elems_vs2.getElement(index).getVal()));
                }
                else if constexpr (opMode == RvvIntegerInsts::OperandMode::VX
                                   || opMode == RvvIntegerInsts::OperandMode::WX)
                {
                    elems_vd.getElement(index).setVal(
                        Functor()(static_cast<typename Functor::result_type>(
                                      READ_INT_REG<XLEN>(state, inst->getRs1())),
                                  elems_vs2.getElement(index).getVal()));
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, RvvIntegerInsts::OperandMode opMode, bool isSigned,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::vwiaHandler_(AtlasState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        if constexpr (isSigned)
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return vwiaHelper<XLEN, 8, opMode, FunctorTemp<int16_t>>(state, action_it);
                    break;
                case 16:
                    return vwiaHelper<XLEN, 16, opMode, FunctorTemp<int32_t>>(state, action_it);
                    break;
                case 32:
                    return vwiaHelper<XLEN, 32, opMode, FunctorTemp<int64_t>>(state, action_it);
                    break;
                default:
                    sparta_assert(false, "Invalid SEW value");
                    break;
            }
        }
        else // Unsigned
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return vwiaHelper<XLEN, 8, opMode, FunctorTemp<uint16_t>>(state, action_it);
                    break;
                case 16:
                    return vwiaHelper<XLEN, 16, opMode, FunctorTemp<uint32_t>>(state, action_it);
                    break;
                case 32:
                    return vwiaHelper<XLEN, 32, opMode, FunctorTemp<uint64_t>>(state, action_it);
                    break;
                default:
                    sparta_assert(false, "Invalid SEW value");
                    break;
            }
        }
        return ++action_it;
    }
} // namespace atlas
