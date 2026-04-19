#pragma once

#include <map>

#include "core/Action.hpp"
#include "core/VecElements.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "core/inst_handlers/vector_types.hpp"

namespace pegasus
{
    class PegasusState;

    class RvvIntegerInsts
    {
      public:
        using base_type = RvvIntegerInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      protected:
        // Integer Unary: Ingeter Arithmetic / Bitwise Logical
        // Uses src2 and dst fields
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType viUnaryHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Integer Binary: Ingeter Arithmetic / Bitwise Logical
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType viBinaryHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Integer Multiply-add
        template <typename XLEN, OperandMode opMode, auto func>
        Action::ItrType viTernaryHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

      private:
        // Integer Move
        template <typename XLEN, OperandMode opMode>
        Action::ItrType vimvHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Integer Reverse Subtraction
        template <typename XLEN, OperandMode opMode>
        Action::ItrType virsubHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Integer Widening Signed Unsigned Multiplication
        template <typename XLEN, OperandMode opMode>
        Action::ItrType viwmulsuHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Result for Integer Add-with-carry Subtract-with-borrow
        template <typename XLEN, OperandMode opMode, bool hasMaskOp,
                  template <typename> typename FunctorTemp>
        Action::ItrType viacsbHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Carry/borrow for Integer Add-with-carry Subtract-with-borrow
        template <typename XLEN, OperandMode opMode, bool hasMaskOp, auto detectFuc>
        Action::ItrType vmiacsbHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Integer Compare
        template <typename XLEN, OperandMode opMode, bool isSigned,
                  template <typename> typename FunctorTemp>
        Action::ItrType vmicHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Integer Merge
        template <typename XLEN, OperandMode opMode>
        Action::ItrType vimergeHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // Integer Extension
        template <size_t fracVal, bool isSigned>
        Action::ItrType viextHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };

    template <typename XLEN, size_t elemWidth, OperandMode opMode,
              template <typename> typename FunctorT, typename T>
    Action::ItrType viUnaryHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        sparta_assert(opMode.src1 == OperandMode::Mode::N, "Src1 operand mode should be unused");

        Elements<Element<opMode.src2 == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vs2{state, inst->getVectorConfig(), inst->getRs2()};
        Elements<Element<opMode.dst == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vd{state, inst->getVectorConfig(), inst->getRd()};
        FunctorT<T> functor{};
        using R = typename decltype(elems_vd)::ElemType::ValueType;

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (std::is_signed_v<T>)
                {
                    elems_vd.getElement(index).setVal(
                        static_cast<R>(functor(sext<T>(elems_vs2.getElement(index).getVal()))));
                }
                else
                {
                    elems_vd.getElement(index).setVal(
                        static_cast<R>(functor(zext<T>(elems_vs2.getElement(index).getVal()))));
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, inst->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isSigned,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::viUnaryHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        if constexpr (isSigned)
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return viUnaryHelper<
                        XLEN, 8, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    int16_t, int8_t>>(state, action_it);

                case 16:
                    return viUnaryHelper<
                        XLEN, 16, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    int32_t, int16_t>>(state, action_it);

                case 32:
                    return viUnaryHelper<
                        XLEN, 32, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    int64_t, int32_t>>(state, action_it);

                case 64:
                    if constexpr (opMode.dst == OperandMode::Mode::W
                                  || opMode.src2 == OperandMode::Mode::W)
                    {
                        sparta_assert(false, "Unsupported SEW value");
                    }
                    else
                    {
                        return viUnaryHelper<XLEN, 64, opMode, FunctorTemp, int64_t>(state,
                                                                                     action_it);
                    }
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
                    return viUnaryHelper<
                        XLEN, 8, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    uint16_t, uint8_t>>(state, action_it);

                case 16:
                    return viUnaryHelper<
                        XLEN, 16, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    uint32_t, uint16_t>>(state, action_it);

                case 32:
                    return viUnaryHelper<
                        XLEN, 32, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    uint64_t, uint32_t>>(state, action_it);

                case 64:
                    if constexpr (opMode.dst == OperandMode::Mode::W
                                  || opMode.src2 == OperandMode::Mode::W)
                    {
                        sparta_assert(false, "Unsupported SEW value");
                    }
                    else
                    {
                        return viUnaryHelper<XLEN, 64, opMode, FunctorTemp, uint64_t>(state,
                                                                                      action_it);
                    }
                    break;
                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode,
              template <typename> typename FunctorT, typename T>
    Action::ItrType viBinaryHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        auto elems_vs1 = opMode.src1 != OperandMode::Mode::V
                             ? Elements<Element<elemWidth>, false>{}
                             : Elements<Element<elemWidth>, false>{state, inst->getVectorConfig(),
                                                                   inst->getRs1()};
        Elements<Element<opMode.src2 == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vs2{state, inst->getVectorConfig(), inst->getRs2()};
        Elements<Element<opMode.dst == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vd{state, inst->getVectorConfig(), inst->getRd()};
        FunctorT<T> functor{};
        using R = typename decltype(elems_vd)::ElemType::ValueType;

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    if constexpr (std::is_signed_v<T>)
                    {
                        elems_vd.getElement(index).setVal(
                            static_cast<R>(functor(sext<T>(elems_vs2.getElement(index).getVal()),
                                                   sext<T>(elems_vs1.getElement(index).getVal()))));
                    }
                    else
                    {
                        elems_vd.getElement(index).setVal(
                            static_cast<R>(functor(zext<T>(elems_vs2.getElement(index).getVal()),
                                                   zext<T>(elems_vs1.getElement(index).getVal()))));
                    }
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    if constexpr (std::is_signed_v<T>)
                    {
                        elems_vd.getElement(index).setVal(static_cast<R>(
                            functor(sext<T>(elems_vs2.getElement(index).getVal()),
                                    sext<T>(READ_INT_REG<XLEN>(state, inst->getRs1())))));
                    }
                    else
                    {
                        elems_vd.getElement(index).setVal(static_cast<R>(
                            functor(zext<T>(elems_vs2.getElement(index).getVal()),
                                    zext<T>(READ_INT_REG<XLEN>(state, inst->getRs1())))));
                    }
                }
                else // opMode.src1 == OperandMode::Mode::I
                {
                    if constexpr (std::is_signed_v<T>)
                    {
                        elems_vd.getElement(index).setVal(
                            static_cast<R>(functor(sext<T>(elems_vs2.getElement(index).getVal()),
                                                   sext<T>(inst->getImmediate()))));
                    }
                    else
                    {
                        elems_vd.getElement(index).setVal(
                            static_cast<R>(functor(zext<T>(elems_vs2.getElement(index).getVal()),
                                                   zext<T>(inst->getImmediate()))));
                    }
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vd.begin(), elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, inst->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isSigned,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::viBinaryHandler_(PegasusState* state,
                                                      Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        if constexpr (isSigned)
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return viBinaryHelper<
                        XLEN, 8, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    int16_t, int8_t>>(state, action_it);

                case 16:
                    return viBinaryHelper<
                        XLEN, 16, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    int32_t, int16_t>>(state, action_it);

                case 32:
                    return viBinaryHelper<
                        XLEN, 32, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    int64_t, int32_t>>(state, action_it);

                case 64:
                    if constexpr (opMode.dst == OperandMode::Mode::W
                                  || opMode.src2 == OperandMode::Mode::W)
                    {
                        sparta_assert(false, "Unsupported SEW value");
                    }
                    else
                    {
                        return viBinaryHelper<XLEN, 64, opMode, FunctorTemp, int64_t>(state,
                                                                                      action_it);
                    }
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
                    return viBinaryHelper<
                        XLEN, 8, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    uint16_t, uint8_t>>(state, action_it);

                case 16:
                    return viBinaryHelper<
                        XLEN, 16, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    uint32_t, uint16_t>>(state, action_it);

                case 32:
                    return viBinaryHelper<
                        XLEN, 32, opMode, FunctorTemp,
                        typename std::conditional_t<opMode.dst == OperandMode::Mode::W
                                                        || opMode.src2 == OperandMode::Mode::W,
                                                    uint64_t, uint32_t>>(state, action_it);

                case 64:
                    if constexpr (opMode.dst == OperandMode::Mode::W
                                  || opMode.src2 == OperandMode::Mode::W)
                    {
                        sparta_assert(false, "Unsupported SEW value");
                    }
                    else
                    {
                        return viBinaryHelper<XLEN, 64, opMode, FunctorTemp, uint64_t>(state,
                                                                                       action_it);
                    }
                    break;
                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        return ++action_it;
    }

} // namespace pegasus
