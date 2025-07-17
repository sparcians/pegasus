#include "core/inst_handlers/v/RvvReductionInsts.hpp"
#include "core/AtlasState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace atlas {

    // Register instruction handlers
    template <typename XLEN>
    void RvvReductionInsts::getInstHandlers(std::map<std::string, Action>& inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vredsum.vs",
            atlas::Action::createAction<&RvvReductionInsts::vredsumvsHandler_<std::plus>,
                                        RvvReductionInsts>(
                nullptr,
                "vredsum.vs",
                ActionTags::EXECUTE_TAG));
    }

    // Template instantiations for both RV32 and RV64
    template void RvvReductionInsts::getInstHandlers<RV32>(std::map<std::string, Action>&);
    template void RvvReductionInsts::getInstHandlers<RV64>(std::map<std::string, Action>&);

    // Helper function for actual reduction logic
    // template <size_t ElemWidth, typename Functor>
    // Action::ItrType vredsumvsHelper(AtlasState* state, Action::ItrType action_it)
    // {
    //     const AtlasInstPtr& inst = state->getCurrentInst();
    //     Elements<Element<ElemWidth>, false> elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
    //     Elements<Element<ElemWidth>, false> elems_vs1{state, state->getVectorConfig(), inst->getRs1()};
    //     Elements<Element<ElemWidth>, false> elems_vd{state, state->getVectorConfig(), inst->getRd()};

    //     auto accumulator = elems_vs1.getElement(0).getVal();

    //     if (inst->getVM()) // unmasked
    //     {
    //         for (auto iter = elems_vs2.begin(); iter != elems_vs2.end(); ++iter)
    //         {
    //             const auto idx = iter.getIndex();
    //             accumulator = Functor()(accumulator, elems_vs2.getElement(idx).getVal());
    //         }
    //     }
    //     else // masked
    //     {
    //         const MaskElements mask_elems{state, state->getVectorConfig(), atlas::V0};
    //         auto bit_iter = mask_elems.maskBitIterBegin();
    //         for (auto iter = elems_vs2.begin(); iter != elems_vs2.end() && bit_iter != mask_elems.maskBitIterEnd(); ++iter, ++bit_iter)
    //         {
    //             if (*bit_iter)
    //             {
    //                 const auto idx = iter.getIndex();
    //                 accumulator = Functor()(accumulator, elems_vs2.getElement(idx).getVal());
    //             }
    //         }
    //     }

    //     elems_vd.getElement(0).setVal(accumulator);
    //     return ++action_it;
    // }

template <size_t ElemWidth, typename Functor>
Action::ItrType vredsumvsHelper(AtlasState* state, Action::ItrType action_it)
{
    const AtlasInstPtr& inst = state->getCurrentInst();
    Elements<Element<ElemWidth>, false> elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
    Elements<Element<ElemWidth>, false> elems_vs1{state, state->getVectorConfig(), inst->getRs1()};
    Elements<Element<ElemWidth>, false> elems_vd{state, state->getVectorConfig(), inst->getRd()};

    auto accumulator = elems_vs1.getElement(0).getVal();

    auto execute = [&]<typename Iterator>(Iterator begin, Iterator end)
    {
        for (auto iter = begin; iter != end; ++iter)
        {
                const auto idx = iter.getIndex();
                accumulator = Functor{}(accumulator, elems_vs2.getElement(idx).getVal());
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

    elems_vd.getElement(0).setVal(accumulator);
    return ++action_it;
}



    // Dispatch SEW-sized implementation of vredsum.vs
    template <template <typename> typename OP>
    Action::ItrType RvvReductionInsts::vredsumvsHandler_(AtlasState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vredsumvsHelper<8, OP<uint8_t>>(state, action_it);
            case 16:
                return vredsumvsHelper<16, OP<uint16_t>>(state, action_it);
            case 32:
                return vredsumvsHelper<32, OP<uint32_t>>(state, action_it);
            case 64:
                return vredsumvsHelper<64, OP<uint64_t>>(state, action_it);
            default:
                sparta_assert(false, "Invalid SEW in vredsum.vs");
        }

        return ++action_it;
    }

} // namespace atlas
