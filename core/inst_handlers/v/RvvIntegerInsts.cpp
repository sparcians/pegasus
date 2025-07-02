#include <limits>
#include <functional>

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
                &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VV, std::plus>, RvvIntegerInsts>(
                nullptr, "vadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VX, std::plus>, RvvIntegerInsts>(
                nullptr, "vadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vi",
            atlas::Action::createAction<
                &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VI, std::plus>, RvvIntegerInsts>(
                nullptr, "vadd.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vand.vv", atlas::Action::createAction<
                           &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VV, std::bit_and>,
                           RvvIntegerInsts>(nullptr, "vand.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vand.vx", atlas::Action::createAction<
                           &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VX, std::bit_and>,
                           RvvIntegerInsts>(nullptr, "vand.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vand.vi", atlas::Action::createAction<
                           &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VI, std::bit_and>,
                           RvvIntegerInsts>(nullptr, "vand.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vor.vv", atlas::Action::createAction<
                          &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VV, std::bit_or>,
                          RvvIntegerInsts>(nullptr, "vor.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vor.vx", atlas::Action::createAction<
                          &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VX, std::bit_or>,
                          RvvIntegerInsts>(nullptr, "vor.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vor.vi", atlas::Action::createAction<
                          &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VI, std::bit_or>,
                          RvvIntegerInsts>(nullptr, "vor.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vxor.vv", atlas::Action::createAction<
                           &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VV, std::bit_xor>,
                           RvvIntegerInsts>(nullptr, "vxor.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vxor.vx", atlas::Action::createAction<
                           &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VX, std::bit_xor>,
                           RvvIntegerInsts>(nullptr, "vxor.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vxor.vi", atlas::Action::createAction<
                           &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VI, std::bit_xor>,
                           RvvIntegerInsts>(nullptr, "vxor.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsub.vv", atlas::Action::createAction<
                           &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VV, std::minus>,
                           RvvIntegerInsts>(nullptr, "vsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsub.vx", atlas::Action::createAction<
                           &RvvIntegerInsts::viablHandler_<XLEN, OperandMode::VX, std::minus>,
                           RvvIntegerInsts>(nullptr, "vsub.vx", ActionTags::EXECUTE_TAG));

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

        inst_handlers.emplace(
            "vadc.vvm",
            atlas::Action::createAction<
                &RvvIntegerInsts::viacsbHandler_<XLEN, OperandMode::VV, true, std::plus>,
                RvvIntegerInsts>(nullptr, "vadc.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadc.vxm",
            atlas::Action::createAction<
                &RvvIntegerInsts::viacsbHandler_<XLEN, OperandMode::VX, true, std::plus>,
                RvvIntegerInsts>(nullptr, "vadc.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadc.vim",
            atlas::Action::createAction<
                &RvvIntegerInsts::viacsbHandler_<XLEN, OperandMode::VI, true, std::plus>,
                RvvIntegerInsts>(nullptr, "vadc.vim", ActionTags::EXECUTE_TAG));

        auto carryFn = []<typename T>(T a, T b, T c)
        { return a >= std::numeric_limits<T>::max() - b - c; };
        inst_handlers.emplace(
            "vmadc.vvm",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VV, true, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vxm",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VX, true, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vim",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VI, true, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vim", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VV, false, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VX, false, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vi",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VI, false, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vi", ActionTags::EXECUTE_TAG));

        auto borrowFn = []<typename T>(T a, T b, T c)
        { return (b + c == std::numeric_limits<T>::max()) || (a < b + c); };
        inst_handlers.emplace(
            "vmsbc.vvm",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VV, true, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsbc.vxm",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VX, true, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsbc.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VV, false, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsbc.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<XLEN, OperandMode::VX, false, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmseq.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VV, true, std::equal_to>,
                RvvIntegerInsts>(nullptr, "vmseq.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmseq.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VX, true, std::equal_to>,
                RvvIntegerInsts>(nullptr, "vmseq.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmseq.vi",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VI, true, std::equal_to>,
                RvvIntegerInsts>(nullptr, "vmseq.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsne.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VV, true, std::not_equal_to>,
                RvvIntegerInsts>(nullptr, "vmsne.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsne.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VX, true, std::not_equal_to>,
                RvvIntegerInsts>(nullptr, "vmsne.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsne.vi",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VI, true, std::not_equal_to>,
                RvvIntegerInsts>(nullptr, "vmsne.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsltu.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VV, false, std::less>,
                RvvIntegerInsts>(nullptr, "vmsltu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsltu.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VX, false, std::less>,
                RvvIntegerInsts>(nullptr, "vmsltu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmslt.vv", atlas::Action::createAction<
                            &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VV, true, std::less>,
                            RvvIntegerInsts>(nullptr, "vmslt.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmslt.vx", atlas::Action::createAction<
                            &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VX, true, std::less>,
                            RvvIntegerInsts>(nullptr, "vmslt.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsleu.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VV, false, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsleu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsleu.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VX, false, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsleu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsleu.vi",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VI, false, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsleu.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsle.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VV, true, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsle.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsle.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VX, true, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsle.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsle.vi",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VI, true, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsle.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsgtu.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VV, false, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgtu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgtu.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VX, false, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgtu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsgt.vv",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VV, true, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgt.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgt.vx",
            atlas::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<XLEN, OperandMode::VX, true, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgt.vx", ActionTags::EXECUTE_TAG));
    }

    template void RvvIntegerInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvIntegerInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, RvvIntegerInsts::OperandMode opMode,
              typename Functor>
    Action::ItrType viablHelper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
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
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  elems_vs1.getElement(index).getVal()));
                }
                else if (opMode == RvvIntegerInsts::OperandMode::VX)
                {
                    elems_vd.getElement(index).setVal(
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  static_cast<typename Functor::result_type>(
                                      READ_INT_REG<XLEN>(state, inst->getRs1()))));
                }
                else // OperandMode::VI
                {
                    elems_vd.getElement(index).setVal(Functor()(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<typename Functor::result_type>(inst->getImmediate())));
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
    Action::ItrType RvvIntegerInsts::viablHandler_(AtlasState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return viablHelper<XLEN, 8, opMode, FunctorTemp<uint8_t>>(state, action_it);
                break;
            case 16:
                return viablHelper<XLEN, 16, opMode, FunctorTemp<uint16_t>>(state, action_it);
                break;
            case 32:
                return viablHelper<XLEN, 32, opMode, FunctorTemp<uint32_t>>(state, action_it);
                break;
            case 64:
                return viablHelper<XLEN, 64, opMode, FunctorTemp<uint64_t>>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvIntegerInsts::OperandMode opMode,
              typename Functor>
    Action::ItrType vwiaHelper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        constexpr size_t elemWidthVs2 = (opMode == RvvIntegerInsts::OperandMode::WX
                                         || opMode == RvvIntegerInsts::OperandMode::WV)
                                            ? 2 * elemWidth
                                            : elemWidth;
        Elements<Element<elemWidthVs2>, false> elems_vs2{state, state->getVectorConfig(),
                                                         inst->getRs2()};
        Elements<Element<2 * elemWidth>, false> elems_vd{state, state->getVectorConfig(),
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
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  elems_vs1.getElement(index).getVal()));
                }
                else if constexpr (opMode == RvvIntegerInsts::OperandMode::VX
                                   || opMode == RvvIntegerInsts::OperandMode::WX)
                {
                    elems_vd.getElement(index).setVal(
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  static_cast<typename Functor::result_type>(
                                      READ_INT_REG<XLEN>(state, inst->getRs1()))));
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
                    sparta_assert(false, "Unsupported SEW value");
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
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvIntegerInsts::OperandMode opMode, bool hasMaskOp,
              typename Functor>
    Action::ItrType viacsbHelper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        MaskElements elems_v0{state, state->getVectorConfig(), atlas::V0};
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};

        for (auto iter = elems_vs2.begin(); iter != elems_vs2.end(); ++iter)
        {
            auto index = iter.getIndex();
            typename Element<elemWidth>::ValueType value = 0;
            if constexpr (opMode == RvvIntegerInsts::OperandMode::VV)
            {
                value = Functor()(elems_vs2.getElement(index).getVal(),
                                  elems_vs1.getElement(index).getVal());
            }
            else if constexpr (opMode == RvvIntegerInsts::OperandMode::VX)
            {
                value = Functor()(elems_vs2.getElement(index).getVal(),
                                  static_cast<typename Functor::result_type>(
                                      READ_INT_REG<XLEN>(state, inst->getRs1())));
            }
            else // OperandMode::VI
            {
                value = Functor()(elems_vs2.getElement(index).getVal(),
                                  static_cast<typename Functor::result_type>(inst->getImmediate()));
            }
            if constexpr (hasMaskOp)
            {
                value = Functor()(
                    value, static_cast<typename Functor::result_type>(elems_v0.getBit(index)));
            }
            elems_vd.getElement(index).setVal(value);
        }

        return ++action_it;
    }

    template <typename XLEN, RvvIntegerInsts::OperandMode opMode, bool hasMaskOp,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::viacsbHandler_(AtlasState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return viacsbHelper<XLEN, 8, opMode, hasMaskOp, FunctorTemp<uint8_t>>(state,
                                                                                      action_it);
                break;
            case 16:
                return viacsbHelper<XLEN, 16, opMode, hasMaskOp, FunctorTemp<uint16_t>>(state,
                                                                                        action_it);
                break;
            case 32:
                return viacsbHelper<XLEN, 32, opMode, hasMaskOp, FunctorTemp<uint32_t>>(state,
                                                                                        action_it);
                break;
            case 64:
                return viacsbHelper<XLEN, 64, opMode, hasMaskOp, FunctorTemp<uint64_t>>(state,
                                                                                        action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvIntegerInsts::OperandMode opMode, bool hasMaskOp,
              auto detectFunc>
    Action::ItrType vmiacsbHelper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        MaskElements elems_v0{state, state->getVectorConfig(), atlas::V0};
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        MaskElements elems_vd{state, state->getVectorConfig(), inst->getRd()};

        for (auto iter = elems_vs2.begin(); iter != elems_vs2.end(); ++iter)
        {
            auto index = iter.getIndex();
            auto a = elems_vs2.getElement(index).getVal();
            decltype(a) b = 0, c = 0;
            if constexpr (opMode == RvvIntegerInsts::OperandMode::VV)
            {
                b = elems_vs1.getElement(index).getVal();
            }
            else if constexpr (opMode == RvvIntegerInsts::OperandMode::VX)
            {
                b = static_cast<decltype(b)>(READ_INT_REG<XLEN>(state, inst->getRs1()));
            }
            else // OperandMode::VI
            {
                b = static_cast<decltype(b)>(inst->getImmediate());
            }
            if constexpr (hasMaskOp)
            {
                c = static_cast<decltype(b)>(elems_v0.getBit(index));
            }
            elems_vd.getElement(index).setVal(detectFunc(a, b, c));
        }

        return ++action_it;
    }

    template <typename XLEN, RvvIntegerInsts::OperandMode opMode, bool hasMaskOp, auto detectFunc>
    Action::ItrType RvvIntegerInsts::vmiacsbHandler_(AtlasState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vmiacsbHelper<XLEN, 8, opMode, hasMaskOp, detectFunc>(state, action_it);
                break;
            case 16:
                return vmiacsbHelper<XLEN, 16, opMode, hasMaskOp, detectFunc>(state, action_it);
                break;
            case 32:
                return vmiacsbHelper<XLEN, 32, opMode, hasMaskOp, detectFunc>(state, action_it);
                break;
            case 64:
                return vmiacsbHelper<XLEN, 64, opMode, hasMaskOp, detectFunc>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvIntegerInsts::OperandMode opMode,
              typename Functor>
    Action::ItrType vmicHelper(AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        MaskElements elems_vd{state, state->getVectorConfig(), inst->getRd()};

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            size_t index = 0;
            for (auto iter = begin; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode == RvvIntegerInsts::OperandMode::VV)
                {
                    elems_vd.getElement(index).setBit(
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  elems_vs1.getElement(index).getVal()));
                }
                else if (opMode == RvvIntegerInsts::OperandMode::VX)
                {
                    elems_vd.getElement(index).setBit(
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else // OperandMode::VI
                {
                    elems_vd.getElement(index).setBit(
                        Functor()(elems_vs2.getElement(index).getVal(), inst->getImmediate()));
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
    Action::ItrType RvvIntegerInsts::vmicHandler_(AtlasState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        if constexpr (isSigned)
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return vmicHelper<XLEN, 8, opMode, FunctorTemp<int8_t>>(state, action_it);
                    break;
                case 16:
                    return vmicHelper<XLEN, 16, opMode, FunctorTemp<int16_t>>(state, action_it);
                    break;
                case 32:
                    return vmicHelper<XLEN, 32, opMode, FunctorTemp<int32_t>>(state, action_it);
                    break;
                case 64:
                    return vmicHelper<XLEN, 64, opMode, FunctorTemp<int64_t>>(state, action_it);
                    break;
                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        else // Unsigned
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return vmicHelper<XLEN, 8, opMode, FunctorTemp<uint8_t>>(state, action_it);
                    break;
                case 16:
                    return vmicHelper<XLEN, 16, opMode, FunctorTemp<uint16_t>>(state, action_it);
                    break;
                case 32:
                    return vmicHelper<XLEN, 32, opMode, FunctorTemp<uint32_t>>(state, action_it);
                    break;
                case 64:
                    return vmicHelper<XLEN, 64, opMode, FunctorTemp<uint64_t>>(state, action_it);
                    break;
                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        return ++action_it;
    }

} // namespace atlas
