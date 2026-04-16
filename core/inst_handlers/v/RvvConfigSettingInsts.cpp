#include "core/inst_handlers/v/RvvConfigSettingInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    // VectorConfig::vsetAVL and VectorConfig::vsetVTYPE are only used in this file
    // Defining them here avoids linker errors

    /**
     * @brief Sets the active vector length (AVL) for the RISC-V vector extension.
     *
     * This function calculates and sets the active vector length (VL) based on the
     * provided AVL value and the maximum vector length (VLMAX). It also updates the
     * corresponding CSR (Control and Status Register) for VL.
     *
     * @tparam XLEN The width of the integer registers (e.g., 32 or 64 bits).
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param set_max Boolean flag indicating whether to set VL to the maximum value (VLMAX).
     * @param avl The requested active vector length.
     * @return XLEN The resulting active vector length (VL).
     */
    template <typename XLEN> XLEN VectorConfig::vsetAVL(PegasusState* state, bool set_max, XLEN avl)
    {
        const size_t vl = set_max ? getVLMAX() : std::min<size_t>(getVLMAX(), avl);
        setVL(vl);
        WRITE_CSR_REG<XLEN>(state, VL, vl);
        return vl;
    }

    /**
     * @brief Configures the vector unit's VTYPE register and updates related settings.
     *
     * This function writes the given VTYPE value to the VTYPE CSR register, decodes
     * the VLMUL field to determine the LMUL (vector length multiplier), and sets
     * various vector configuration parameters such as SEW (standard element width),
     * VTA (vector tail agnostic), and VMA (vector mask agnostic).
     *
     * @tparam XLEN The width of the general-purpose registers (e.g., 32 or 64 bits).
     * @param state Pointer to the PegasusState object representing the current state.
     * @param vtype The value to be written to the VTYPE CSR register.
     *
     * @throws std::runtime_error If the VLMUL field in the VTYPE register contains an invalid
     * encoding.
     */
    template <typename XLEN> void VectorConfig::vsetVTYPE(PegasusState* state, XLEN vtype)
    {
        WRITE_CSR_REG<XLEN>(state, VTYPE, vtype);
        const size_t vlmul = READ_CSR_FIELD<XLEN>(state, VTYPE, "vlmul");

        static const size_t lmul_table[8] = {
            8,  // 000
            16, // 001
            32, // 010
            64, // 011
            0,  // 100 (invalid)
            1,  // 101
            2,  // 110
            4   // 111
        };

        const size_t lmul = lmul_table[vlmul & 0b111];
        sparta_assert(lmul, "Invalid vtype VLMUL encoding.");
        setLMUL(lmul);
        setSEW(8u << READ_CSR_FIELD<XLEN>(state, VTYPE, "vsew"));
        setVTA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vta"));
        setVMA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vma"));
    }

    template RV32 VectorConfig::vsetAVL<RV32>(PegasusState*, bool, RV32);
    template RV64 VectorConfig::vsetAVL<RV64>(PegasusState*, bool, RV64);
    template void VectorConfig::vsetVTYPE<RV32>(PegasusState*, RV32);
    template void VectorConfig::vsetVTYPE<RV64>(PegasusState*, RV64);

    template <typename XLEN>
    void RvvConfigSettingInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vsetvl", pegasus::Action::createAction<&RvvConfigSettingInsts::vsetvlHandler_<XLEN>,
                                                    RvvConfigSettingInsts>(
                          nullptr, "vsetvl", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsetvli", pegasus::Action::createAction<&RvvConfigSettingInsts::vsetvliHandler_<XLEN>,
                                                     RvvConfigSettingInsts>(
                           nullptr, "vsetvli", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsetivli",
            pegasus::Action::createAction<&RvvConfigSettingInsts::vsetivliHandler_<XLEN>,
                                          RvvConfigSettingInsts>(nullptr, "vsetivli",
                                                                 ActionTags::EXECUTE_TAG));
    }

    template void RvvConfigSettingInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvConfigSettingInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    /**
     * @brief Handles the `vsetvl` instruction, which sets the vector type (VTYPE) and
     *        active vector length (AVL) for the RISC-V vector extension.
     *
     * This function updates the vector configuration based on the values in the source
     * registers (rs1 and rs2). It writes the resulting vector length (VL) to the destination
     * register (rd) and performs a configuration check.
     *
     * - If rs1 is zero, AVL is set to the maximum vector length, and VL is written to rd.
     * - If rs1 is non-zero, AVL is set to the value in rs1, and VL is updated accordingly.
     *
     * @tparam XLEN The width of the integer registers (e.g., 32 or 64 bits).
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <typename XLEN>
    Action::ItrType RvvConfigSettingInsts::vsetvlHandler_(pegasus::PegasusState* state,
                                                          Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN vtype_val = READ_INT_REG<XLEN>(state, inst->getRs2());
        VectorConfig* vector_config = state->getVectorConfig();
        vector_config->vsetVTYPE<XLEN>(state, vtype_val);
        if (inst->getRs1() == 0)
        {
            if (inst->getRd() != 0)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    vector_config->vsetAVL<XLEN>(state, true));
            }
        }
        else
        {
            vector_config->vsetAVL<XLEN>(state, false, READ_INT_REG<XLEN>(state, inst->getRs1()));
            WRITE_INT_REG<XLEN>(state, inst->getRd(), vector_config->getVL());
        }
        vector_config->checkConfig();
        return ++action_it;
    }

    /**
     * @brief Handles the `vsetvli` instruction, which sets the vector type (VTYPE) and
     *        active vector length (AVL) for the RISC-V vector extension.
     *
     * This function updates the vector configuration based on the immediate value
     * (VTYPE) and the value in the source register (rs1). It writes the resulting
     * vector length (VL) to the destination register (rd) and performs a configuration check.
     *
     * - If rs1 is zero, AVL is set to the maximum vector length, and VL is written to rd.
     * - If rs1 is non-zero, AVL is set to the value in rs1, and VL is updated accordingly.
     *
     * @tparam XLEN The width of the integer registers (e.g., 32 or 64 bits).
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <typename XLEN>
    Action::ItrType RvvConfigSettingInsts::vsetvliHandler_(pegasus::PegasusState* state,
                                                           Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN vtype_val = inst->getImmediate();
        VectorConfig* vector_config = state->getVectorConfig();
        vector_config->vsetVTYPE<XLEN>(state, vtype_val);
        if (inst->getRs1() == 0)
        {
            if (inst->getRd() != 0)
            {
                WRITE_INT_REG<XLEN>(state, inst->getRd(),
                                    vector_config->vsetAVL<XLEN>(state, true));
            }
        }
        else
        {
            vector_config->vsetAVL<XLEN>(state, false, READ_INT_REG<XLEN>(state, inst->getRs1()));
            WRITE_INT_REG<XLEN>(state, inst->getRd(), vector_config->getVL());
        }
        vector_config->checkConfig();
        return ++action_it;
    }

    /**
     * @brief Handles the `vsetivli` instruction, which sets the vector type (VTYPE) and
     *        active vector length (AVL) for the RISC-V vector extension.
     *
     * This function updates the vector configuration based on the immediate value
     * and AVL provided by the instruction. It writes the resulting vector length (VL)
     * to the destination register and performs a configuration check.
     *
     * @tparam XLEN The width of the integer registers (e.g., 32 or 64 bits).
     * @param state Pointer to the current PegasusState, which holds the processor state.
     * @param action_it Iterator pointing to the current action in the action list.
     * @return Action::ItrType Iterator pointing to the next action in the action list.
     */
    template <typename XLEN>
    Action::ItrType RvvConfigSettingInsts::vsetivliHandler_(pegasus::PegasusState* state,
                                                            Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN vtype_val = inst->getImmediate();
        VectorConfig* vector_config = state->getVectorConfig();
        vector_config->vsetVTYPE<XLEN>(state, vtype_val);
        vector_config->vsetAVL<XLEN>(state, false, inst->getAVL());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), vector_config->getVL());
        vector_config->checkConfig();
        return ++action_it;
    }

} // namespace pegasus
