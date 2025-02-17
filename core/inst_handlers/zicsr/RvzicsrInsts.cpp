#include "core/inst_handlers/inst_helpers.hpp"
#include "arch/register_macros.hpp"
#include "core/inst_handlers/zicsr/RvzicsrInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/Exception.hpp"
#include "core/Trap.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvzicsrInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "csrrc",
                atlas::Action::createAction<&RvzicsrInsts::csrrc_handler<RV64>, RvzicsrInsts>(
                    nullptr, "csrrc", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrci",
                atlas::Action::createAction<&RvzicsrInsts::csrrci_handler<RV64>, RvzicsrInsts>(
                    nullptr, "csrrci", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrs",
                atlas::Action::createAction<&RvzicsrInsts::csrrs_handler<RV64>, RvzicsrInsts>(
                    nullptr, "csrrs", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrsi",
                atlas::Action::createAction<&RvzicsrInsts::csrrsi_handler<RV64>, RvzicsrInsts>(
                    nullptr, "csrrsi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrw",
                atlas::Action::createAction<&RvzicsrInsts::csrrw_handler<RV64>, RvzicsrInsts>(
                    nullptr, "csrrw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrwi",
                atlas::Action::createAction<&RvzicsrInsts::csrrwi_handler<RV64>, RvzicsrInsts>(
                    nullptr, "csrrwi", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "csrrc",
                atlas::Action::createAction<&RvzicsrInsts::csrrc_handler<RV32>, RvzicsrInsts>(
                    nullptr, "csrrc", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrci",
                atlas::Action::createAction<&RvzicsrInsts::csrrci_handler<RV32>, RvzicsrInsts>(
                    nullptr, "csrrci", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrs",
                atlas::Action::createAction<&RvzicsrInsts::csrrs_handler<RV32>, RvzicsrInsts>(
                    nullptr, "csrrs", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrsi",
                atlas::Action::createAction<&RvzicsrInsts::csrrsi_handler<RV32>, RvzicsrInsts>(
                    nullptr, "csrrsi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrw",
                atlas::Action::createAction<&RvzicsrInsts::csrrw_handler<RV32>, RvzicsrInsts>(
                    nullptr, "csrrw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrwi",
                atlas::Action::createAction<&RvzicsrInsts::csrrwi_handler<RV32>, RvzicsrInsts>(
                    nullptr, "csrrwi", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzicsrInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzicsrInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrc_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const auto rs1 = insn->getRs1();
        auto rd = insn->getRd();
        const uint32_t csr =
            insn->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        // Don't write CSR is rs1=x0
        if (rs1 != 0)
        {
            // rs1 value is treated as a bit mask to clear bits
            const XLEN rs1_val = READ_INT_REG<XLEN>(state, rs1);
            WRITE_CSR_REG<XLEN>(state, csr, (~rs1_val & csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrci_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN imm = insn->getImmediate();
        const auto rd = insn->getRd();
        const int csr =
            insn->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        if (imm)
        {
            WRITE_CSR_REG<XLEN>(state, csr, (~imm & csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrs_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const auto rs1 = insn->getRs1();
        const auto rd = insn->getRd();
        const int csr =
            insn->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        if (rs1 != 0)
        {
            // rs1 value is treated as a bit mask to set bits
            const XLEN rs1_val = READ_INT_REG<XLEN>(state, rs1);
            WRITE_CSR_REG<XLEN>(state, csr, (rs1_val | csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrsi_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN imm = insn->getImmediate();
        const auto rd = insn->getRd();
        const int csr =
            insn->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        if (imm)
        {
            // imm value is treated as a bit mask
            WRITE_CSR_REG<XLEN>(state, csr, (imm | csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const auto rs1 = insn->getRs1();
        const auto rd = insn->getRd();
        const int csr =
            insn->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, rs1);
        // Only read CSR if rd!=x0
        if (rd != 0)
        {
            const XLEN csr_val = zext(READ_CSR_REG<XLEN>(state, csr), state->getXlen());
            WRITE_INT_REG<XLEN>(state, rd, csr_val);
        }

        WRITE_CSR_REG<XLEN>(state, csr, rs1_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrwi_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN imm = insn->getImmediate();
        const auto rd = insn->getRd();
        const int csr =
            insn->getMavisOpcodeInfo()->getSpecialField(mavis::OpcodeInfo::SpecialField::CSR);

        // Only read CSR if rd!=x0
        if (rd != 0)
        {
            const XLEN csr_val = zext(READ_CSR_REG<XLEN>(state, csr), state->getXlen());
            WRITE_INT_REG<XLEN>(state, rd, csr_val);
        }

        WRITE_CSR_REG<XLEN>(state, csr, imm);

        return nullptr;
    }
} // namespace atlas
