#include "core/inst_handlers/v/RvvPermuteInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "core/inst_handlers/finst_helpers.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvPermuteInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vmv.s.x",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::S,
                                                                .src1 = OperandMode::Mode::X}>,
                RvvPermuteInsts>(nullptr, "vmv.s.x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv.x.s",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::X,
                                                                .src2 = OperandMode::Mode::S}>,
                RvvPermuteInsts>(nullptr, "vmv.x.s", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfmv.s.f",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::S,
                                                                .src1 = OperandMode::Mode::F}>,
                RvvPermuteInsts>(nullptr, "vfmv.s.f", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmv.f.s",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vmvHandler_<XLEN, OperandMode{.dst = OperandMode::Mode::F,
                                                                .src2 = OperandMode::Mode::S}>,
                RvvPermuteInsts>(nullptr, "vfmv.f.s", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vslideup.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslideHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::X},
                                                 true>,
                RvvPermuteInsts>(nullptr, "vslideup.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vslideup.vi",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslideHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::I},
                                                 true>,
                RvvPermuteInsts>(nullptr, "vslideup.vi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vslidedown.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslideHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::X},
                                                 false>,
                RvvPermuteInsts>(nullptr, "vslidedown.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vslidedown.vi",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslideHandler_<XLEN,
                                                 OperandMode{.dst = OperandMode::Mode::V,
                                                             .src2 = OperandMode::Mode::V,
                                                             .src1 = OperandMode::Mode::I},
                                                 false>,
                RvvPermuteInsts>(nullptr, "vslidedown.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vslide1up.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslide1Handler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::X},
                                                  true>,
                RvvPermuteInsts>(nullptr, "vslide1up.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfslide1up.vf",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslide1Handler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::F},
                                                  true>,
                RvvPermuteInsts>(nullptr, "vfslide1up.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vslide1down.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslide1Handler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::X},
                                                  false>,
                RvvPermuteInsts>(nullptr, "vslide1down.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfslide1down.vf",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vslide1Handler_<XLEN,
                                                  OperandMode{.dst = OperandMode::Mode::V,
                                                              .src2 = OperandMode::Mode::V,
                                                              .src1 = OperandMode::Mode::F},
                                                  false>,
                RvvPermuteInsts>(nullptr, "vfslide1down.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vrgather.vv",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vrgatherHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   false>,
                RvvPermuteInsts>(nullptr, "vrgather.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vrgather.vx",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vrgatherHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::X},
                                                   false>,
                RvvPermuteInsts>(nullptr, "vrgather.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vrgather.vi",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vrgatherHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::I},
                                                   false>,
                RvvPermuteInsts>(nullptr, "vrgather.vi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vrgatherei16.vv",
            pegasus::Action::createAction<
                &RvvPermuteInsts::vrgatherHandler_<XLEN,
                                                   OperandMode{.dst = OperandMode::Mode::V,
                                                               .src2 = OperandMode::Mode::V,
                                                               .src1 = OperandMode::Mode::V},
                                                   true>,
                RvvPermuteInsts>(nullptr, "vrgatherei16.vv", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vcompress.vm",
            pegasus::Action::createAction<&RvvPermuteInsts::vcompressHandler_, RvvPermuteInsts>(
                nullptr, "vcompress.vm", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmv1r.v",
            pegasus::Action::createAction<&RvvPermuteInsts::vmvrHandler_<1>, RvvPermuteInsts>(
                nullptr, "vmv1r.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv2r.v",
            pegasus::Action::createAction<&RvvPermuteInsts::vmvrHandler_<2>, RvvPermuteInsts>(
                nullptr, "vmv2r.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv4r.v",
            pegasus::Action::createAction<&RvvPermuteInsts::vmvrHandler_<4>, RvvPermuteInsts>(
                nullptr, "vmv4r.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmv8r.v",
            pegasus::Action::createAction<&RvvPermuteInsts::vmvrHandler_<8>, RvvPermuteInsts>(
                nullptr, "vmv8r.v", ActionTags::EXECUTE_TAG));
    }

    template void RvvPermuteInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvPermuteInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    /**
     * @brief Helper function to handle RISC-V vector move instructions between element 0 and scalar
     * registers.
     *
     * This function supports all RISC-V vector move instructions that involve moving data
     * between element 0 of a vector register and scalar registers. It handles instructions
     * such as `vmv.x.s` (move element 0 to a scalar register) and `vmv.s.x` (move a scalar
     * value to element 0 of a vector register). The operation is determined by the specific
     * instruction being executed.
     *
     * @tparam XLEN The width of the scalar registers (e.g., 32 or 64 bits). Determines the size of
     * scalar values used in operations.
     * @tparam elemWidth The width of each vector element (e.g., 16, 32, or 64 bits). Specifies the
     * size of the floating-point elements in the vector register.
     * @tparam opMode Specifies the operand mode for the operation. Determines the source of the
     * second operand:
     *                - Immediate Mode: The second operand is an immediate constant.
     *                - Scalar Register Mode: The second operand is loaded from a scalar register.
     *                - Vector Mode: The second operand is another vector register.
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <typename XLEN, size_t elemWidth, OperandMode opMode>
    Action::ItrType vmvHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        using ValueType = typename Element<elemWidth>::ValueType;

        if constexpr ((opMode.dst == OperandMode::Mode::F || opMode.dst == OperandMode::Mode::X)
                      && opMode.src2 == OperandMode::Mode::S)
        {
            Elements<Element<elemWidth>, false> elems_vs2{state, inst->getVectorConfig(),
                                                          inst->getRs2()};
            if constexpr (opMode.dst == OperandMode::Mode::X)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    sext<XLEN>(elems_vs2.getElement(0).getVal()));
            }
            else // opMode.dst == OperandMode::Mode::F
            {
                WRITE_FP_REG<RV64>(state, inst->getRd(),
                                   nanBoxing<RV64, ValueType>(elems_vs2.getElement(0).getVal()));
            }
        }
        else if constexpr ((opMode.src1 == OperandMode::Mode::F
                            || opMode.src1 == OperandMode::Mode::X)
                           && opMode.dst == OperandMode::Mode::S)
        {
            if (vector_config->getVSTART() < vector_config->getVL())
            {
                Elements<Element<elemWidth>, false> elems_vd{state, inst->getVectorConfig(),
                                                             inst->getRd()};
                if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(0).setVal(
                        sext<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else // opMode.src1 == OperandMode::Mode::F
                {
                    elems_vd.getElement(0).setVal(
                        checkNanBoxing<ValueType, RV64>(READ_FP_REG<RV64>(state, inst->getRs1())));
                }
            }
        }
        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode>
    Action::ItrType RvvPermuteInsts::vmvHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                if constexpr (opMode.dst == OperandMode::Mode::F
                              || opMode.src1 == OperandMode::Mode::F)
                {
                    break;
                }
                // vmv.x.s or vmv.s.x
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

    /**
     * @brief Helper function to handle RISC-V vector slide instructions.
     *
     * This function implements the behavior of RISC-V vector slide instructions, such as
     * `vslideup.vx`, `vslideup.vi`, `vslidedown.vx`, and `vslidedown.vi`. These instructions
     * shift elements within a vector register up or down by a specified amount.
     *
     * The operation is applied element-wise across the vector register, and the specific
     * behavior (e.g., slide up or slide down) is determined by the instruction being executed.
     *
     * @tparam XLEN The width of the scalar registers (e.g., 32 or 64 bits). Determines the size of
     * scalar values used in operations.
     * @tparam elemWidth The width of each vector element (e.g., 16, 32, or 64 bits). Specifies the
     * size of the floating-point elements in the vector register.
     * @tparam opMode Specifies the operand mode for the operation. Determines the source of the
     * second operand:
     *                - Immediate Mode: The second operand is an immediate constant.
     *                - Scalar Register Mode: The second operand is loaded from a scalar register.
     *                - Vector Mode: The second operand is another vector register.
     * @tparam isUp A boolean template parameter that indicates the direction of the slide. If
     * `true`, the elements are slid up; if `false`, they are slid down.
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool isUp>
    Action::ItrType vslideHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const size_t offset = opMode.src1 == OperandMode::Mode::I
                                  ? inst->getImmediate()
                                  : READ_INT_REG<XLEN>(state, inst->getRs1());
        Elements<Element<elemWidth>, false> elems_vs2{state, inst->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, inst->getVectorConfig(), inst->getRd()};

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (isUp)
                {
                    elems_vd.getElement(index).setVal(
                        elems_vs2.getElement(index - offset).getVal());
                }
                else
                {
                    using ValueType = typename Element<elemWidth>::ValueType;
                    ValueType val = (index + offset) < inst->getVectorConfig()->getVLMAX()
                                        ? elems_vs2.getElement(index + offset).getVal()
                                        : 0;
                    elems_vd.getElement(index).setVal(val);
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            auto begin = isUp ? typename decltype(elems_vd)::ElementIterator(&elems_vd, offset)
                              : elems_vd.begin();
            execute(begin, elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, inst->getVectorConfig(), pegasus::V0};
            auto begin =
                isUp ? MaskBitIterator(&mask_elems, offset) : mask_elems.maskBitIterBegin();
            execute(begin, mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isUp>
    Action::ItrType RvvPermuteInsts::vslideHandler_(pegasus::PegasusState* state,
                                                    Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vslideHelper<XLEN, 8, opMode, isUp>(state, action_it);

            case 16:
                return vslideHelper<XLEN, 16, opMode, isUp>(state, action_it);

            case 32:
                return vslideHelper<XLEN, 32, opMode, isUp>(state, action_it);

            case 64:
                return vslideHelper<XLEN, 64, opMode, isUp>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    /**
     * @brief Handles the RISC-V `vslide1` instruction, which performs a single-element slide
     * operation.
     *
     * This function implements the `vslide1` instruction, which shifts elements within a vector
     * register up or down by one position. The direction of the slide (up or down) is determined by
     * the `isUp` template parameter. The operation is applied element-wise across the vector
     * register, and the vacated position is filled with the value from the neighboring element.
     *
     * @tparam XLEN The width of the scalar registers (e.g., 32 or 64 bits).
     * @tparam elemWidth The width of each vector element (e.g., 8, 16, 32, or 64 bits).
     * @tparam opMode The operand mode, which determines the source of the slide (e.g., vector or
     * scalar).
     * @tparam isUp A boolean indicating the direction of the slide (`true` for up, `false` for
     * down).
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool isUp>
    Action::ItrType vslide1Helper(PegasusState* state, Action::ItrType action_it)
    {
        using ValueType = typename Element<elemWidth>::ValueType;
        const PegasusInstPtr & inst = state->getCurrentInst();
        const size_t offset = 1;
        Elements<Element<elemWidth>, false> elems_vs2{state, inst->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, inst->getVectorConfig(), inst->getRd()};

        auto execute = [&](auto iter, const auto & end)
        {
            const auto vl = inst->getVectorConfig()->getVL();
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (isUp)
                {
                    elems_vd.getElement(index).setVal(
                        elems_vs2.getElement(index - offset).getVal());
                }
                else
                {
                    ValueType val = (index + offset) < inst->getVectorConfig()->getVLMAX()
                                        ? elems_vs2.getElement(index + offset).getVal()
                                        : 0;
                    elems_vd.getElement(index).setVal(val);
                }
            }
            if (!isUp && index == vl - 1)
            {
                if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(index).setVal(
                        sext<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else
                {
                    elems_vd.getElement(index).setVal(READ_FP_REG<RV64>(state, inst->getRs1()));
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            if (isUp && inst->getVectorConfig()->getVSTART() == 0)
            {
                if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(0).setVal(
                        sext<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else
                {
                    elems_vd.getElement(0).setVal(READ_FP_REG<RV64>(state, inst->getRs1()));
                }
            }
            auto begin = isUp ? typename decltype(elems_vd)::ElementIterator(&elems_vd, offset)
                              : elems_vd.begin();
            execute(begin, elems_vd.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, inst->getVectorConfig(), pegasus::V0};
            if (isUp && inst->getVectorConfig()->getVSTART() == 0 && mask_elems.getBit(0))
            {
                if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(0).setVal(
                        sext<ValueType>(READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else
                {
                    elems_vd.getElement(0).setVal(READ_FP_REG<RV64>(state, inst->getRs1()));
                }
            }
            auto begin =
                isUp ? MaskBitIterator(&mask_elems, offset) : mask_elems.maskBitIterBegin();
            execute(begin, mask_elems.maskBitIterEnd());
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isUp>
    Action::ItrType RvvPermuteInsts::vslide1Handler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vslide1Helper<XLEN, 8, opMode, isUp>(state, action_it);

            case 16:
                return vslide1Helper<XLEN, 16, opMode, isUp>(state, action_it);

            case 32:
                return vslide1Helper<XLEN, 32, opMode, isUp>(state, action_it);

            case 64:
                return vslide1Helper<XLEN, 64, opMode, isUp>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    /**
     * @brief Handles the RISC-V `vrgather` instructions (`vrgather.vi`, `vrgather.vx`,
     * `vrgather.vv`), which perform vector element gathering.
     *
     * This function implements the `vrgather` family of instructions, including:
     * - `vrgather.vi`: Gathers elements from a source vector register based on an immediate index.
     * - `vrgather.vx`: Gathers elements from a source vector register based on indices provided in
     * a scalar register.
     * - `vrgather.vv`: Gathers elements from a source vector register based on indices provided in
     * another vector register.
     *
     * The operation is applied element-wise, and the gathered elements are stored in the
     * destination vector register. If an index is out of bounds (i.e., greater than or equal to the
     * number of elements in the source vector), the corresponding destination element is set to
     * `0`. Masking can be applied to conditionally perform the operation on specific elements based
     * on the active mask bits.
     *
     * @tparam XLEN The width of the scalar registers (e.g., 32 or 64 bits). Determines the size of
     * scalar values used in operations.
     * @tparam elemWidth The width of each vector element (e.g., 16, 32, or 64 bits). Specifies the
     * size of the floating-point elements in the vector register.
     * @tparam opMode Specifies the operand mode for the operation. Determines the source of the
     * second operand:
     *                - Immediate Mode: The second operand is an immediate constant.
     *                - Scalar Register Mode: The second operand is loaded from a scalar register.
     *                - Vector Mode: The second operand is another vector register.
     * @tparam isIndex16 A boolean template parameter that indicates whether the indices are 16-bit
     * values (true) or the same width as the vector elements (false).
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool is16>
    Action::ItrType vrgatherHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs2{state, inst->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<is16 ? 16 : elemWidth>, false> elems_vs1{state, inst->getVectorConfig(),
                                                                  inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vd{state, inst->getVectorConfig(), inst->getRd()};
        size_t i = opMode.src1 == OperandMode::Mode::I   ? inst->getImmediate()
                   : opMode.src1 == OperandMode::Mode::X ? READ_INT_REG<XLEN>(state, inst->getRs1())
                                                         : 0;

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if (opMode.src1 == OperandMode::Mode::V)
                {
                    i = elems_vs1.getElement(index).getVal();
                }
                if (i >= inst->getVectorConfig()->getVLMAX())
                {
                    elems_vd.getElement(index).setVal(0);
                }
                else
                {
                    elems_vd.getElement(index).setVal(elems_vs2.getElement(i).getVal());
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

    template <typename XLEN, OperandMode opMode, bool is16>
    Action::ItrType RvvPermuteInsts::vrgatherHandler_(pegasus::PegasusState* state,
                                                      Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vrgatherHelper<XLEN, 8, opMode, is16>(state, action_it);

            case 16:
                return vrgatherHelper<XLEN, 16, opMode, is16>(state, action_it);

            case 32:
                return vrgatherHelper<XLEN, 32, opMode, is16>(state, action_it);

            case 64:
                return vrgatherHelper<XLEN, 64, opMode, is16>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    /**
     * @brief Handles the RISC-V `vcompress` instruction, which performs active element compression.
     *
     * This function implements the `vcompress` instruction, which compresses elements from a source
     * vector register into a destination vector register based on the active mask bits in the mask
     * register `v0`. Only elements corresponding to active mask bits (`1`) are selected and stored
     * contiguously in the destination vector register. The operation is applied element-wise across
     * the vector registers.
     *
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <size_t elemWidth>
    Action::ItrType vcompressHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs2{state, inst->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, inst->getVectorConfig(), inst->getRd()};
        const MaskElements mask_elems{state, inst->getVectorConfig(), inst->getRs1()};
        size_t i = 0;

        for (auto iter = mask_elems.maskBitIterBegin(); iter != mask_elems.maskBitIterEnd();
             ++iter, ++i)
        {
            elems_vd.getElement(i).setVal(elems_vs2.getElement(iter.getIndex()).getVal());
        }

        return ++action_it;
    }

    Action::ItrType RvvPermuteInsts::vcompressHandler_(pegasus::PegasusState* state,
                                                       Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vcompressHelper<8>(state, action_it);

            case 16:
                return vcompressHelper<16>(state, action_it);

            case 32:
                return vcompressHelper<32>(state, action_it);

            case 64:
                return vcompressHelper<64>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    /**
     * @brief Handles the RISC-V `vmv?r.v` instruction, which performs whole register element
     * movement.
     *
     * The operation is unconditional and moves all elements from the source vector to the
     * destination vector.
     *
     * @tparam elemCount The number of elements to move, determined by the specific instruction
     * variant (e.g., 1, 2, 4, or 8).
     * @tparam nRegs The number of vector registers involved in the operation, determined by the
     * specific instruction variant (e.g., 1 for `vmv1r.v`, 2 for `vmv2r.v`, etc.).
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <size_t elemWidth, size_t nReg>
    Action::ItrType vmvrHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        VectorConfig* config = inst->getVectorConfig();
        Elements<Element<elemWidth>, false> elems_vs2{state, config, inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, config, inst->getRd()};

        for (auto iter = elems_vd.begin(); iter != elems_vd.end(); ++iter)
        {
            auto index = iter.getIndex();
            elems_vd.getElement(index).setVal(elems_vs2.getElement(index).getVal());
        }

        return ++action_it;
    }

    template <size_t nReg>
    Action::ItrType RvvPermuteInsts::vmvrHandler_(pegasus::PegasusState* state,
                                                  Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const VectorConfig* vector_config = inst->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vmvrHelper<8, nReg>(state, action_it);

            case 16:
                return vmvrHelper<16, nReg>(state, action_it);

            case 32:
                return vmvrHelper<32, nReg>(state, action_it);

            case 64:
                return vmvrHelper<64, nReg>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }
} // namespace pegasus
