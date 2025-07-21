#include "core/inst_handlers/v/RvvPermuteInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvvPermuteInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vmv.s.x", atlas::Action::createAction<
                           &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{OperandMode::Mode::V,
                                                                           OperandMode::Mode::N,
                                                                           OperandMode::Mode::X}>,
                           RvvPermuteInsts>(nullptr, "vmv.s.x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv.x.s", atlas::Action::createAction<
                           &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{OperandMode::Mode::X,
                                                                           OperandMode::Mode::V,
                                                                           OperandMode::Mode::N}>,
                           RvvPermuteInsts>(nullptr, "vmv.x.s", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfmv.s.f", atlas::Action::createAction<
                            &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{OperandMode::Mode::V,
                                                                            OperandMode::Mode::N,
                                                                            OperandMode::Mode::F}>,
                            RvvPermuteInsts>(nullptr, "vfmv.s.x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmv.f.s", atlas::Action::createAction<
                            &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{OperandMode::Mode::F,
                                                                            OperandMode::Mode::V,
                                                                            OperandMode::Mode::N}>,
                            RvvPermuteInsts>(nullptr, "vfmv.x.s", ActionTags::EXECUTE_TAG));
    }

    template void RvvPermuteInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvPermuteInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, OperandMode opMode>
    Action::ItrType vmvHelper(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr inst = state->getCurrentInst();
        VectorConfig* vector_config = state->getVectorConfig();

        if constexpr ((opMode.dst == OperandMode::Mode::F || opMode.dst == OperandMode::Mode::X)
                      && opMode.src2 == OperandMode::Mode::V)
        {
            Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                          inst->getRs2()};
            if constexpr (opMode.dst == OperandMode::Mode::X)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    sextu<XLEN>(elems_vs2.getElement(0).getVal()));
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
                        sextu<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
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
    Action::ItrType RvvPermuteInsts::vmvHandler_(atlas::AtlasState* state,
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

} // namespace atlas
