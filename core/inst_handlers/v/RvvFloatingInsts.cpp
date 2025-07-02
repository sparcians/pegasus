#include <limits>

#include "core/inst_handlers/v/RvvFloatingInsts.hpp"
#include "core/inst_handlers/RvfInstsHelpers.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvvFloatingInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vfadd.vv",
            atlas::Action::createAction<&RvvFloatingInsts::vfaddHandler_<XLEN>, RvvFloatingInsts>(
                nullptr, "vfadd.vv", ActionTags::EXECUTE_TAG));
    }

    template void RvvFloatingInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvFloatingInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth>
    Action::ItrType vfaddHelper(atlas::AtlasState* state, Action::ItrType action_it)
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

                if constexpr (elemWidth == 32)
                {
                    auto val = f32_add(float32_t{elems_vs1.getElement(index).getVal()},
                                       float32_t{elems_vs2.getElement(index).getVal()})
                                   .v;
                    elems_vd.getElement(index).setVal(val);
                }
                else // 64
                {
                    auto val = f64_add(float64_t{elems_vs1.getElement(index).getVal()},
                                       float64_t{elems_vs2.getElement(index).getVal()})
                                   .v;
                    elems_vd.getElement(index).setVal(val);
                }
            }
            floatUpdateCsr<XLEN>(state);
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs1.begin(), elems_vs1.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvFloatingInsts::vfaddHandler_(atlas::AtlasState* state,
                                                    Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 32:
                return vfaddHelper<XLEN, 32>(state, action_it);
                break;
            case 64:
                return vfaddHelper<XLEN, 64>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }
} // namespace atlas
