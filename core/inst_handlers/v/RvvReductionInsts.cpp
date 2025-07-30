#include "core/inst_handlers/v/RvvReductionInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{

    // Register instruction handlers
    template <typename XLEN>
    void RvvReductionInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vredsum.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredsumvsHandler_<std::plus>,
                                          RvvReductionInsts>(nullptr, "vredsum.vs",
                                                             ActionTags::EXECUTE_TAG));
    }

    // Template instantiations for both RV32 and RV64
    template void RvvReductionInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvReductionInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <size_t ElemWidth, typename Functor>
    Action::ItrType vredsumvsHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<ElemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<ElemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<ElemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};

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
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        elems_vd.getElement(0).setVal(
            accumulator); // TODO: Support tail agnostic/undisturbed policy as a parameter.
                          // Currently assuming undisturbed (requires vd as a source).
                          // For tail-agnostic, we'll likely write all 1's or some deterministic
                          // pattern to tail elements. This should be configurable via vector policy
                          // (vta).
        return ++action_it;
    }

    // Dispatch SEW-sized implementation of vredsum.vs
    template <template <typename> typename OP>
    Action::ItrType RvvReductionInsts::vredsumvsHandler_(PegasusState* state,
                                                         Action::ItrType action_it)
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

} // namespace pegasus
