#include <limits>

#include "core/inst_handlers/v/RvvMaskInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvMaskInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        using ValueType = UintType<MaskElements::ElemType::elem_width>;

        inst_handlers.emplace(
            "vmand.mm",
            pegasus::Action::createAction<
                &RvvMaskInsts::vmlHandler_<[](ValueType a, ValueType b) { return a & b; }>,
                RvvMaskInsts>(nullptr, "vmand.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmnand.mm",
            pegasus::Action::createAction<
                &RvvMaskInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~(a & b); }>,
                RvvMaskInsts>(nullptr, "vmnand.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmandn.mm",
            pegasus::Action::createAction<
                &RvvMaskInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~a & b; }>,
                RvvMaskInsts>(nullptr, "vmandn.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmor.mm",
            pegasus::Action::createAction<
                &RvvMaskInsts::vmlHandler_<[](ValueType a, ValueType b) { return a | b; }>,
                RvvMaskInsts>(nullptr, "vmor.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmnor.mm",
            pegasus::Action::createAction<
                &RvvMaskInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~(a | b); }>,
                RvvMaskInsts>(nullptr, "vmnor.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmorn.mm",
            pegasus::Action::createAction<
                &RvvMaskInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~a | b; }>,
                RvvMaskInsts>(nullptr, "vmorn.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmxor.mm",
            pegasus::Action::createAction<
                &RvvMaskInsts::vmlHandler_<[](ValueType a, ValueType b) { return a ^ b; }>,
                RvvMaskInsts>(nullptr, "vmxor.mm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmxnor.mm",
            pegasus::Action::createAction<
                &RvvMaskInsts::vmlHandler_<[](ValueType a, ValueType b) { return ~(a ^ b); }>,
                RvvMaskInsts>(nullptr, "vmxnor.mm", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vcpop.m",
            pegasus::Action::createAction<&RvvMaskInsts::vcpHandler_<XLEN>, RvvMaskInsts>(
                nullptr, "vcpop.m", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfirst.m",
            pegasus::Action::createAction<&RvvMaskInsts::vfirstHandler_<XLEN>, RvvMaskInsts>(
                nullptr, "vfirst.m", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsbf.m",
            pegasus::Action::createAction<&RvvMaskInsts::vsxfHandler_<SetFirstMode::BEFORE>,
                                          RvvMaskInsts>(nullptr, "vmsbf.m",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsif.m",
            pegasus::Action::createAction<&RvvMaskInsts::vsxfHandler_<SetFirstMode::INCLUDING>,
                                          RvvMaskInsts>(nullptr, "vmsif.m",
                                                        ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsof.m",
            pegasus::Action::createAction<&RvvMaskInsts::vsxfHandler_<SetFirstMode::ONLY>,
                                          RvvMaskInsts>(nullptr, "vmsof.m",
                                                        ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "viota.m", pegasus::Action::createAction<&RvvMaskInsts::viotaHandler_, RvvMaskInsts>(
                           nullptr, "viota.m", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vid.v", pegasus::Action::createAction<&RvvMaskInsts::veiHandler_, RvvMaskInsts>(
                         nullptr, "vid.v", ActionTags::EXECUTE_TAG));
    }

    template void RvvMaskInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvMaskInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    /**
     * @brief Handles the vector mask logical (`ml`) instruction.
     *
     * This function performs logical operations on vector mask registers. The operation
     * is applied element-wise across the mask registers, and the result is stored in the
     * destination mask register. The specific logical operation (e.g., AND, OR, XOR)
     * is determined by the instruction.
     *
     * @tparam func A template parameter that specifies the logical operation to be applied. It is a
     * function that takes two mask element values and returns the result of the logical operation.
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <auto func>
    Action::ItrType RvvMaskInsts::vmlHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr inst = state->getCurrentInst();
        MaskElements elems_vs1{state, inst->getVectorConfig(), inst->getRs1()};
        MaskElements elems_vs2{state, inst->getVectorConfig(), inst->getRs2()};
        MaskElements elems_vd{state, inst->getVectorConfig(), inst->getRd()};

        for (auto iter = elems_vs1.begin(); iter != elems_vs1.end(); ++iter)
        {
            size_t index = iter.getIndex();
            elems_vd.getElement(index).setVal(
                func(elems_vs1.getElement(index).getVal(), elems_vs2.getElement(index).getVal()));
        }

        return ++action_it;
    }

    /**
     * @brief Handles the RISC-V `vcpop.m` instruction, which counts the number of active mask bits.
     *
     * This function implements the `vcpop.m` instruction, which counts the number of bits
     * set to `1` in a mask register. The result is written to a destination scalar register.
     * This operation is typically used to determine the number of active elements in a vector
     * operation based on the mask.
     *
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <typename XLEN>
    Action::ItrType RvvMaskInsts::vcpHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr inst = state->getCurrentInst();
        MaskElements elems_vs2{state, inst->getVectorConfig(), inst->getRs2()};
        MaskElements elems_v0{state, inst->getVectorConfig(), pegasus::V0};
        size_t count = 0;

        for (auto elem_iter = elems_vs2.begin(); elem_iter != elems_vs2.end(); ++elem_iter)
        {
            size_t index = elem_iter.getIndex();
            auto tmp{elems_vs2.getElement(index)};
            if (!inst->getVM())
            {
                tmp.pokeVal(elems_vs2.getElement(index).getVal()
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

    /**
     * @brief Handles the RISC-V `vfirst.m` instruction, which finds the index of the first active
     * mask bit.
     *
     * This function implements the `vfirst.m` instruction, which scans a mask register to find
     * the index of the first bit set to `1`. If no bits are set, the result is `-1`. The result
     * is written to a destination scalar register. This operation is typically used to identify
     * the first active element in a vector operation.
     *
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <typename XLEN>
    Action::ItrType RvvMaskInsts::vfirstHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr inst = state->getCurrentInst();
        MaskElements elems_vs2{state, inst->getVectorConfig(), inst->getRs2()};
        MaskElements elems_v0{state, inst->getVectorConfig(), pegasus::V0};

        for (auto elem_iter = elems_vs2.begin(); elem_iter != elems_vs2.end(); ++elem_iter)
        {
            size_t index = elem_iter.getIndex();
            auto tmp{elems_vs2.getElement(index)};
            if (!inst->getVM())
            {
                tmp.pokeVal(elems_vs2.getElement(index).getVal()
                            & elems_v0.getElement(index).getVal());
            }
            else
            {
                tmp.getVal();
            }
            for (auto bit_iter = tmp.begin(); bit_iter != tmp.end(); ++bit_iter)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    elem_iter.getIndex() * MaskElements::ElemType::elem_width
                                        + bit_iter.getIndex());
                return ++action_it;
            }
        }
        WRITE_INT_REG<XLEN>(state, inst->getRd(), std::numeric_limits<XLEN>::max());

        return ++action_it;
    }

    /**
     * @brief Handles the RISC-V `vsxf` instruction, which performs Vector Mask Set-sfMode
     * instruction
     *
     * This function set active bits in destination mask register under the control of mask register
     * `v0`.
     *
     * The `sfMode` template argument determines the active mode for the bits:
     * - **"including"**: Includes the current element in the detection.
     * - **"before"**: Considers only elements before the current one.
     * - **"only"**: Considers only the current element.
     *
     * @tparam sfMode The detection mode, which determines how bit is selected for the operation
     *                (e.g., "including", "before", or "only").
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <RvvMaskInsts::SetFirstMode sfMode>
    Action::ItrType RvvMaskInsts::vsxfHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr inst = state->getCurrentInst();
        MaskElements elems_vs2{state, inst->getVectorConfig(), inst->getRs2()};
        MaskElements elems_vd{state, inst->getVectorConfig(), inst->getRd()};
        MaskElements elems_v0{state, inst->getVectorConfig(), pegasus::V0};
        bool found = false;

        for (auto elem_iter = elems_vs2.begin(); elem_iter != elems_vs2.end(); ++elem_iter)
        {
            size_t index = elem_iter.getIndex();
            auto elem_v0{elems_v0.getElement(index)};
            auto elem_vd{elems_vd.getElement(index)};
            auto elem_vs2{elems_vs2.getElement(index)};

            if (found) // set all active bits to 0
            {
                MaskElement::ValueType value =
                    inst->getVM() ? 0 : elem_vd.getVal() & ~elem_v0.getVal();
                elem_vd.setVal(value);
                continue;
            }

            if (!inst->getVM())
            {
                elem_vs2.pokeVal(elem_vs2.getVal() & elem_v0.getVal()); // don't store *elem_vs2*
            }
            MaskElement::ValueType value = 0;
            for (auto bit_iter = elem_vs2.begin(); bit_iter != elem_vs2.end(); ++bit_iter)
            {
                // 1 found in current element, compose *value*
                found = true;
                size_t idx = bit_iter.getIndex();
                if constexpr (sfMode == SetFirstMode::BEFORE)
                {
                    value = ((MaskElement::ValueType)1 << idx) - 1;
                }
                else if constexpr (sfMode == SetFirstMode::INCLUDING)
                {
                    value = (((MaskElement::ValueType)1 << idx) - 1)
                            | ((MaskElement::ValueType)1 << idx);
                }
                else // SetFirstMode::ONLY
                {
                    value = (MaskElement::ValueType)1 << idx;
                }
                break;
            }

            // 1 not found in current element, set all bits to 1
            if (!found && sfMode != SetFirstMode::ONLY)
            {
                value = std::numeric_limits<MaskElement::ValueType>::max();
            }

            // only set active bits to *value*
            value = inst->getVM()
                        ? value
                        : (elem_vd.getVal() & ~elem_v0.peekVal()) | (value & elem_v0.peekVal());
            elem_vd.setVal(value);
        }

        return ++action_it;
    }

    /**
     * @brief Handles the RISC-V `viota.m` instruction, which performs index population based on
     * active mask bits.
     *
     * This function implements the `viota.m` instruction, which generates a vector of indices
     * based on the active bits in a mask register. For each active bit (set to `1`) in the mask
     * register, the corresponding element in the destination vector register is populated with
     * the count of preceding active bits. Inactive bits (set to `0`) result in a value of `0`
     * in the destination vector.
     *
     * @tparam elemWidth The width of each vector element (e.g., 8, 16, 32, or 64 bits). Specifies
     * the size of the vector elements being accessed.
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <size_t elemWidth>
    Action::ItrType viotaHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using ElemsType = Elements<Element<elemWidth>, false>;

        const PegasusInstPtr inst = state->getCurrentInst();
        MaskElements elems_vs2{state, inst->getVectorConfig(), inst->getRs2()};
        MaskElements elems_v0{state, inst->getVectorConfig(), pegasus::V0};
        ElemsType elems_vd{state, inst->getVectorConfig(), inst->getRd()};
        size_t count = 0;
        auto iter_v0 = elems_v0.maskBitIterBegin();
        auto iter_vd = elems_vd.begin();

        auto execute = [&](auto & iter, const auto & iter_end)
        {
            for (; iter != iter_end; ++iter)
            {
                elems_vd.getElement(iter.getIndex()).setVal(count);
            }
        };

        for (auto elem_iter = elems_vs2.begin(); elem_iter != elems_vs2.end(); ++elem_iter)
        {
            size_t index = elem_iter.getIndex();
            auto elem_v0{elems_v0.getElement(index)};
            auto elem_vs2{elems_vs2.getElement(index)};

            if (!inst->getVM())
            {
                elem_vs2.pokeVal(elem_vs2.getVal() & elem_v0.getVal()); // don't store *elem_vs2*
            }
            for (auto bit_iter = elem_vs2.begin(); bit_iter != elem_vs2.end(); ++bit_iter)
            {
                size_t idx = bit_iter.getIndex();
                // update parallel prefix sun for each active element of vd till current index
                if (!inst->getVM()) // masked
                {
                    execute(iter_v0,
                            MaskBitIterator{&elems_v0,
                                            MaskElements::ElemType::elem_width * index + idx + 1});
                }
                else
                {
                    typename ElemsType::ElementIterator iter_end{
                        &elems_vd, MaskElements::ElemType::elem_width * index + idx + 1};
                    execute(iter_vd, iter_end);
                }
                ++count;
            }
            // fill in the sum till tail is reached
            if (!inst->getVM()) // masked
            {
                execute(iter_v0, elems_v0.maskBitIterEnd());
            }
            else
            {
                execute(iter_vd, elems_vd.end());
            }
        }

        return ++action_it;
    }

    Action::ItrType RvvMaskInsts::viotaHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return viotaHelper<8>(state, action_it);
                break;
            case 16:
                return viotaHelper<16>(state, action_it);
                break;
            case 32:
                return viotaHelper<32>(state, action_it);
                break;
            case 64:
                return viotaHelper<64>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    /**
     * @brief Handles the RISC-V `vid.v` instruction, which generates element indices for active
     * elements.
     *
     * @tparam elemWidth The width of each vector element (e.g., 8, 16, 32, or 64 bits). Specifies
     * the size of the vector elements being accessed.
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <size_t elemWidth>
    Action::ItrType veiHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using ElemsType = Elements<Element<elemWidth>, false>;

        const PegasusInstPtr inst = state->getCurrentInst();
        ElemsType elems_vd{state, inst->getVectorConfig(), inst->getRd()};
        MaskElements elems_v0{state, inst->getVectorConfig(), pegasus::V0};

        auto execute = [&](auto iter, const auto & iter_end)
        {
            for (; iter != iter_end; ++iter)
            {
                elems_vd.getElement(iter.getIndex()).setVal(iter.getIndex());
            }
        };

        if (!inst->getVM())
        {
            execute(elems_v0.maskBitIterBegin(), elems_v0.maskBitIterEnd());
        }
        else
        {
            execute(elems_vd.begin(), elems_vd.end());
        }

        return ++action_it;
    }

    Action::ItrType RvvMaskInsts::veiHandler_(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return veiHelper<8>(state, action_it);
                break;
            case 16:
                return veiHelper<16>(state, action_it);
                break;
            case 32:
                return veiHelper<32>(state, action_it);
                break;
            case 64:
                return veiHelper<64>(state, action_it);
                break;
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

} // namespace pegasus
