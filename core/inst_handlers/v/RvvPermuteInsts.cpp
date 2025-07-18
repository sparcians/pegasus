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
            "vmv.s.x",
            atlas::Action::createAction<&RvvPermuteInsts::vmvsxHandler_<XLEN>, RvvPermuteInsts>(
                nullptr, "vmv.s.x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv.x.s",
            atlas::Action::createAction<&RvvPermuteInsts::vmvxsHandler_<XLEN>, RvvPermuteInsts>(
                nullptr, "vmv.x.s", ActionTags::EXECUTE_TAG));
    }

    template void RvvPermuteInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvPermuteInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth>
    Action::ItrType vmvsxHelper(atlas::AtlasState* state, Action::ItrType action_it)
    {
        using ValueType = typename Element<elemWidth>::ValueType;

        const AtlasInstPtr inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};
        elems_vd.getElement(0).setVal(sextu<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvPermuteInsts::vmvsxHandler_(atlas::AtlasState* state,
                                                   Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        if (vector_config->getVSTART() < vector_config->getVL())
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return vmvsxHelper<XLEN, 8>(state, action_it);

                case 16:
                    return vmvsxHelper<XLEN, 16>(state, action_it);

                case 32:
                    return vmvsxHelper<XLEN, 32>(state, action_it);

                case 64:
                    return vmvsxHelper<XLEN, 64>(state, action_it);

                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth>
    Action::ItrType vmvxsHelper(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        WRITE_INT_REG<XLEN>(state, inst->getRd(), sextu<XLEN>(elems_vs2.getElement(0).getVal()));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvPermuteInsts::vmvxsHandler_(atlas::AtlasState* state,
                                                   Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vmvxsHelper<XLEN, 8>(state, action_it);

            case 16:
                return vmvxsHelper<XLEN, 16>(state, action_it);

            case 32:
                return vmvxsHelper<XLEN, 32>(state, action_it);

            case 64:
                return vmvxsHelper<XLEN, 64>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

} // namespace atlas
