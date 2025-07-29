#include "core/inst_handlers/v/RvvReductionInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "core/inst_handlers/inst_helpers.hpp"

namespace pegasus
{

    // Register instruction handlers
    template <typename XLEN>
    void RvvReductionInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vredsum.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerUnsigned_<std::plus>,
                                          RvvReductionInsts>(nullptr, "vredsum.vs",
                                                             ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vredand.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerUnsigned_<std::bit_and>,
                                          RvvReductionInsts>(nullptr, "vredand.vs",
                                                             ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vredor.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerUnsigned_<std::bit_or>,
                                          RvvReductionInsts>(nullptr, "vredor.vs",
                                                             ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vredxor.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerUnsigned_<std::bit_xor>,
                                          RvvReductionInsts>(nullptr, "vredxor.vs",
                                                             ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vredmaxu.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerUnsigned_<max_op>,
                                          RvvReductionInsts>(nullptr, "vredmaxu.vs",
                                                             ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vredmax.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerSigned_<max_op>,
                                          RvvReductionInsts>(nullptr, "vredmax.vs",
                                                             ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vredminu.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerUnsigned_<min_op>,
                                          RvvReductionInsts>(nullptr, "vredminu.vs",
                                                             ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vredmin.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerSigned_<min_op>,
                                          RvvReductionInsts>(nullptr, "vredmin.vs",
                                                             ActionTags::EXECUTE_TAG));
    }

    // Template instantiations for both RV32 and RV64
    template void RvvReductionInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvReductionInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <size_t ElemWidth, typename Functor>
    Action::ItrType vredopHelper(PegasusState* state, Action::ItrType action_it)
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

        elems_vd.getElement(0).setVal(accumulator);
        return ++action_it;
    }

    // Dispatch SEW-sized implementation of vector reduction operations
    template <template <typename> typename OP>
    Action::ItrType RvvReductionInsts::vredopHandlerUnsigned_(PegasusState* state,
                                                              Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vredopHelper<8, OP<uint8_t>>(state, action_it);
            case 16:
                return vredopHelper<16, OP<uint16_t>>(state, action_it);
            case 32:
                return vredopHelper<32, OP<uint32_t>>(state, action_it);
            case 64:
                return vredopHelper<64, OP<uint64_t>>(state, action_it);
            default:
                sparta_assert(false, "Unsupported SEW value");
        }

        return ++action_it;
    }

    template <template <typename> typename OP>
    Action::ItrType RvvReductionInsts::vredopHandlerSigned_(PegasusState* state,
                                                            Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vredopHelper<8, OP<int8_t>>(state, action_it);
            case 16:
                return vredopHelper<16, OP<int16_t>>(state, action_it);
            case 32:
                return vredopHelper<32, OP<int32_t>>(state, action_it);
            case 64:
                return vredopHelper<64, OP<int64_t>>(state, action_it);
            default:
                sparta_assert(false, "Unsupported SEW value");
        }

        return ++action_it;
    }

} // namespace pegasus
