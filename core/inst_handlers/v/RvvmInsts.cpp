#include <limits>

#include "core/inst_handlers/v/RvvmInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvvmInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        using ValueType = UintType<VLEN_MIN>;

        inst_handlers.emplace(
            "vmand.mm",
            atlas::Action::createAction<
                &RvvmInsts::vmlHandler_<[](ValueType a, ValueType b) { return a & b; }>, RvvmInsts>(
                nullptr, "vmand.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmnand.mm",
            atlas::Action::createAction<
                &RvvmInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~(a & b); }>,
                RvvmInsts>(nullptr, "vmnand.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmandn.mm",
            atlas::Action::createAction<
                &RvvmInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~a & b; }>,
                RvvmInsts>(nullptr, "vmandn.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmor.mm",
            atlas::Action::createAction<
                &RvvmInsts::vmlHandler_<[](ValueType a, ValueType b) { return a | b; }>, RvvmInsts>(
                nullptr, "vmor.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmnor.mm",
            atlas::Action::createAction<
                &RvvmInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~(a | b); }>,
                RvvmInsts>(nullptr, "vmnor.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmorn.mm",
            atlas::Action::createAction<
                &RvvmInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~a | b; }>,
                RvvmInsts>(nullptr, "vmorn.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmxor.mm",
            atlas::Action::createAction<
                &RvvmInsts::vmlHandler_<[](ValueType a, ValueType b) { return a ^ b; }>, RvvmInsts>(
                nullptr, "vmxor.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmxnor.mm",
            atlas::Action::createAction<
                &RvvmInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~(a ^ b); }>,
                RvvmInsts>(nullptr, "vmxnor.mm", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("vpopc.m",
                              atlas::Action::createAction<&RvvmInsts::vcpHandler_<XLEN>, RvvmInsts>(
                                  nullptr, "vpopc.m", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfirst.m", atlas::Action::createAction<&RvvmInsts::vfirstHandler_<XLEN>, RvvmInsts>(
                            nullptr, "vfirst.m", ActionTags::EXECUTE_TAG));
    }

    template void RvvmInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvmInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <auto func>
    Action::ItrType RvvmInsts::vmlHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr inst = state->getCurrentInst();
        MaskElements elems_vs1{state, state->getVectorConfig(), inst->getRs1()};
        MaskElements elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        MaskElements elems_vd{state, state->getVectorConfig(), inst->getRd()};

        for (auto iter = elems_vs1.begin(); iter != elems_vs1.end(); ++iter)
        {
            size_t index = iter.getIndex();
            elems_vd.getElement(index).setVal(
                func(elems_vs1.getElement(index).getVal(), elems_vs2.getElement(index).getVal()));
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvmInsts::vcpHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr inst = state->getCurrentInst();
        MaskElements elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        MaskElements elems_v0{state, state->getVectorConfig(), atlas::V0};
        size_t count = 0;

        for (auto elem_iter = elems_vs2.begin(); elem_iter != elems_vs2.end(); ++elem_iter)
        {
            size_t index = elem_iter.getIndex();
            auto tmp{elems_vs2.getElement(index)};
            if (!inst->getVM())
            {
                tmp.setVal(elems_vs2.getElement(index).getVal()
                           & elems_v0.getElement(index).getVal());
            }
            for (auto bit_iter = tmp.begin(); bit_iter != tmp.end(); ++bit_iter)
            {
                ++count;
            }
        }
        WRITE_INT_REG<XLEN>(state, inst->getRd(), count);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvmInsts::vfirstHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr inst = state->getCurrentInst();
        MaskElements elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        MaskElements elems_v0{state, state->getVectorConfig(), atlas::V0};

        for (auto elem_iter = elems_vs2.begin(); elem_iter != elems_vs2.end(); ++elem_iter)
        {
            size_t index = elem_iter.getIndex();
            auto tmp{elems_vs2.getElement(index)};
            if (!inst->getVM())
            {
                tmp.setVal(elems_vs2.getElement(index).getVal()
                           & elems_v0.getElement(index).getVal());
            }
            for (auto bit_iter = tmp.begin(); bit_iter != tmp.end(); ++bit_iter)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    elem_iter.getIndex() * VLEN_MIN + bit_iter.getIndex());
                return ++action_it;
            }
        }
        WRITE_INT_REG<XLEN>(state, inst->getRd(), std::numeric_limits<XLEN>::max());

        return ++action_it;
    }
} // namespace atlas
