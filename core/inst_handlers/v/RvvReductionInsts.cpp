#include "core/inst_handlers/v/RvvReductionInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "core/inst_handlers/i/RviFunctors.hpp"
#include "core/inst_handlers/f/RvfFunctors.hpp"
#include <climits>

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
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerUnsigned_<Max>,
                                          RvvReductionInsts>(nullptr, "vredmaxu.vs",
                                                             ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vredmax.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerSigned_<Max>,
                                          RvvReductionInsts>(nullptr, "vredmax.vs",
                                                             ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vredminu.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerUnsigned_<Min>,
                                          RvvReductionInsts>(nullptr, "vredminu.vs",
                                                             ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vredmin.vs",
            pegasus::Action::createAction<&RvvReductionInsts::vredopHandlerSigned_<Min>,
                                          RvvReductionInsts>(nullptr, "vredmin.vs",
                                                             ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwredsumu.vs",
            pegasus::Action::createAction<
                &RvvReductionInsts::vwredopHandlerUnsigned_<std::plus>, RvvReductionInsts>(
                nullptr, "vwredsumu.vs", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwredsum.vs",
            pegasus::Action::createAction<
                &RvvReductionInsts::vwredopHandlerSigned_<std::plus>, RvvReductionInsts>(
                nullptr, "vwredsum.vs", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("vfredosum.vs", Action::createAction<&RvvReductionInsts::vfredopHandler_<Fadd>,
                                                               RvvReductionInsts>(nullptr, "vfredosum.vs", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("vfredusum.vs", Action::createAction<&RvvReductionInsts::vfredopHandler_<Fadd>,
                                                                RvvReductionInsts>(nullptr, "vfredusum.vs", ActionTags::EXECUTE_TAG));

    }

    // Template instantiations for both RV32 and RV64
    template void RvvReductionInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvReductionInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename inType, typename outType, typename Functor>
    Action::ItrType vredopHelper(PegasusState* state, Action::ItrType action_it)
    {
        static constexpr auto inWidth = sizeof(inType) * CHAR_BIT;
        static constexpr auto outWidth = sizeof(outType) * CHAR_BIT;

        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<inWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                    inst->getRs2()};
        Elements<Element<inWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                    inst->getRs1()};
        Elements<Element<outWidth>, false> elems_vd{state, state->getVectorConfig(), inst->getRd()};

        outType accumulator = static_cast<outType>(elems_vs1.getElement(0).getVal());

        auto execute = [&]<typename Iterator>(Iterator begin, Iterator end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                const auto idx = iter.getIndex();
                accumulator = Functor{}(accumulator,
                                        static_cast<outType>(elems_vs2.getElement(idx).getVal()));
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

    template <typename inType, typename outType, auto Functor>
    Action::ItrType vfredopHelper(PegasusState* state, Action::ItrType action_it)
    {
        static constexpr auto inWidth = sizeof(inType) * CHAR_BIT;
        static constexpr auto outWidth = sizeof(outType) * CHAR_BIT;

        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<inWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                    inst->getRs2()};
        Elements<Element<inWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                    inst->getRs1()};
        Elements<Element<outWidth>, false> elems_vd{state, state->getVectorConfig(), inst->getRd()};

        typename Element<inWidth>::ValueType accumulator = elems_vs1.getElement(0).getVal();

        auto execute = [&]<typename Iterator>(Iterator begin, Iterator end)
        {
            for (auto iter = begin; iter != end; ++iter)
            {
                const auto idx = iter.getIndex();
                accumulator = Functor(accumulator,
                                        (static_cast<UintType<outWidth>>(elems_vs2.getElement(idx).getVal())));
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
    Action::ItrType RvvReductionInsts::vredopHandlerUnsigned_(PegasusState* state,
                                                              Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vredopHelper<uint8_t, uint8_t, OP<uint8_t>>(state, action_it);
            case 16:
                return vredopHelper<uint16_t, uint16_t, OP<uint16_t>>(state, action_it);
            case 32:
                return vredopHelper<uint32_t, uint32_t, OP<uint32_t>>(state, action_it);
            case 64:
                return vredopHelper<uint64_t, uint64_t, OP<uint64_t>>(state, action_it);
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
                return vredopHelper<int8_t, int8_t, OP<int8_t>>(state, action_it);
            case 16:
                return vredopHelper<int16_t, int16_t, OP<int16_t>>(state, action_it);
            case 32:
                return vredopHelper<int32_t, int32_t, OP<int32_t>>(state, action_it);
            case 64:
                return vredopHelper<int64_t, int64_t, OP<int64_t>>(state, action_it);
            default:
                sparta_assert(false, "Unsupported SEW value");
        }

        return ++action_it;
    }

    template <template <typename> typename OP>
    Action::ItrType RvvReductionInsts::vwredopHandlerUnsigned_(PegasusState* state,
                                                                      Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vredopHelper<uint8_t, uint16_t, OP<uint16_t>>(state, action_it);
            case 16:
                return vredopHelper<uint16_t, uint32_t, OP<uint32_t>>(state, action_it);
            case 32:
                return vredopHelper<uint32_t, uint64_t, OP<uint64_t>>(state, action_it);
            case 64:
                sparta_assert(false, "Widening from SEW=64 to 128 bits is invalid: element sizes > "
                                     "64 bits are not supported");
            default:
                sparta_assert(false, "Unsupported SEW value");
        }

        return ++action_it;
    }

    template <template <typename> typename OP>
    Action::ItrType RvvReductionInsts::vwredopHandlerSigned_(PegasusState* state,
                                                                    Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vredopHelper<int8_t, int16_t, OP<int16_t>>(state, action_it);
            case 16:
                return vredopHelper<int16_t, int32_t, OP<int32_t>>(state, action_it);
            case 32:
                return vredopHelper<int32_t, int64_t, OP<int64_t>>(state, action_it);
            case 64:
                sparta_assert(false, "Widening from SEW=64 to 128 bits is invalid: element sizes > "
                                     "64 bits are not supported");
            default:
                sparta_assert(false, "Unsupported SEW value");
        }

        return ++action_it;
    }

    template <template <typename> typename OP>
    Action::ItrType RvvReductionInsts::vfredopHandler_(PegasusState* state,
                                                              Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfredopHelper<float16_t, float16_t, [](auto src1, auto src2)
                                                            {
                                                                return OP<float16_t>{}(float16_t{src1}, float16_t{src2}).v;
                                                            }
                                        >(state, action_it);
            case 32:
                return vfredopHelper<float32_t, float32_t, [](auto src1, auto src2)
                                                            {
                                                                return OP<float32_t>{}(float32_t{src1}, float32_t{src2}).v;
                                                            }
                                        >(state, action_it);
            case 64:
                return vfredopHelper<float64_t, float64_t, [](auto src1, auto src2)
                                                            {
                                                                return OP<float64_t>{}(float64_t{src1}, float64_t{src2}).v;
                                                            }
                                        >(state, action_it);
            default:
                sparta_assert(false, "Unsupported SEW value");
        }

        return ++action_it;
    }

} // namespace pegasus
