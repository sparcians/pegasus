#include <limits>
#include <type_traits>

#include "core/inst_handlers/v/RvvFloatInsts.hpp"
#include "core/inst_handlers/finsts_helpers.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvvFloatInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vfadd.vv",
            atlas::Action::createAction<&RvvFloatInsts::vfaddHandler_<XLEN, OperandMode::VV>,
                                        RvvFloatInsts>(nullptr, "vfadd.vv",
                                                       ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfadd.vf",
            atlas::Action::createAction<&RvvFloatInsts::vfaddHandler_<XLEN, OperandMode::VF>,
                                        RvvFloatInsts>(nullptr, "vfadd.vf",
                                                       ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsub.vv",
            atlas::Action::createAction<&RvvFloatInsts::vfsubHandler_<XLEN, OperandMode::VV>,
                                        RvvFloatInsts>(nullptr, "vfsub.vv",
                                                       ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsub.vf",
            atlas::Action::createAction<&RvvFloatInsts::vfsubHandler_<XLEN, OperandMode::VF>,
                                        RvvFloatInsts>(nullptr, "vfsub.vf",
                                                       ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfrsub.vf",
            atlas::Action::createAction<&RvvFloatInsts::vfrsubHandler_<XLEN>, RvvFloatInsts>(
                nullptr, "vfrsub.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfmacc.vv",
            atlas::Action::createAction<&RvvFloatInsts::vfmaccHandler_<XLEN, OperandMode::VV>,
                                        RvvFloatInsts>(nullptr, "vfmacc.vv",
                                                       ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmacc.vf",
            atlas::Action::createAction<&RvvFloatInsts::vfmaccHandler_<XLEN, OperandMode::VF>,
                                        RvvFloatInsts>(nullptr, "vfmacc.vf",
                                                       ActionTags::EXECUTE_TAG));
    }

    template void RvvFloatInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvFloatInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, RvvFloatInsts::OperandMode opMode, auto func>
    Action::ItrType vfbinaryHelper(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                typename Element<elemWidth>::ValueType value = 0;
                if constexpr (opMode == RvvFloatInsts::OperandMode::VV)
                {
                    value = func(elems_vs2.getElement(index).getVal(),
                                 elems_vs1.getElement(index).getVal());
                }
                else if constexpr (opMode == RvvFloatInsts::OperandMode::VF)
                {
                    value = func(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<UintType<elemWidth>>(READ_FP_REG<RV64>(state, inst->getRs1())));
                }
                elems_vd.getElement(index).setVal(value);
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

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, RvvFloatInsts::OperandMode opMode>
    Action::ItrType RvvFloatInsts::vfaddHandler_(atlas::AtlasState* state,
                                                 Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 32:
                return vfbinaryHelper<XLEN, 32, opMode, [](auto a, auto b) {
                    return f32_add(float32_t{a}, float32_t{b}).v;
                }>(state, action_it);
                break;
            case 64:
                return vfbinaryHelper<XLEN, 64, opMode, [](auto a, auto b) {
                    return f64_add(float64_t{a}, float64_t{b}).v;
                }>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, RvvFloatInsts::OperandMode opMode>
    Action::ItrType RvvFloatInsts::vfsubHandler_(atlas::AtlasState* state,
                                                 Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 32:
                return vfbinaryHelper<XLEN, 32, opMode, [](auto a, auto b) {
                    return f32_sub(float32_t{a}, float32_t{b}).v;
                }>(state, action_it);
                break;
            case 64:
                return vfbinaryHelper<XLEN, 64, opMode, [](auto a, auto b) {
                    return f64_sub(float64_t{a}, float64_t{b}).v;
                }>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvFloatInsts::vfrsubHandler_(atlas::AtlasState* state,
                                                  Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 32:
                return vfbinaryHelper<XLEN, 32, OperandMode::VF, [](auto a, auto b) {
                    return f32_sub(float32_t{b}, float32_t{a}).v;
                }>(state, action_it);
                break;
            case 64:
                return vfbinaryHelper<XLEN, 64, OperandMode::VF, [](auto a, auto b) {
                    return f64_sub(float64_t{b}, float64_t{a}).v;
                }>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, RvvFloatInsts::OperandMode opMode, auto func>
    Action::ItrType vfternaryHelper(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                auto elem_vd = elems_vd.getElement(index);
                typename Element<elemWidth>::ValueType value = 0;
                if constexpr (opMode == RvvFloatInsts::OperandMode::VV)
                {
                    value = func(elems_vs2.getElement(index).getVal(),
                                 elems_vs1.getElement(index).getVal(), elem_vd.getVal());
                }
                else if constexpr (opMode == RvvFloatInsts::OperandMode::VF)
                {
                    value = func(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<UintType<elemWidth>>(READ_FP_REG<RV64>(state, inst->getRs1())),
                        elem_vd.getVal());
                }
                elem_vd.setVal(value);
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

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, RvvFloatInsts::OperandMode opMode>
    Action::ItrType RvvFloatInsts::vfmaccHandler_(atlas::AtlasState* state,
                                                  Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 32:
                return vfternaryHelper<XLEN, 32, opMode, [](auto src2, auto src1, auto dst) {
                    return f32_mulAdd(float32_t{src2}, float32_t{src1}, float32_t{dst}).v;
                }>(state, action_it);
                break;
            case 64:
                return vfternaryHelper<XLEN, 64, opMode, [](auto src2, auto src1, auto dst) {
                    return f64_mulAdd(float64_t{src2}, float64_t{src1}, float64_t{dst}).v;
                }>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }
} // namespace atlas
