#include "core/inst_handlers/rv64/zicsr/RvzicsrInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "arch/register_macros.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/Exception.hpp"
#include "core/Trap.hpp"

namespace atlas
{
    void RvzicsrInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        inst_handlers.emplace(
            "csrrc", atlas::Action::createAction<&RvzicsrInsts::csrrc_64_handler, RvzicsrInsts>(
                         nullptr, "csrrc", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrci", atlas::Action::createAction<&RvzicsrInsts::csrrci_64_handler, RvzicsrInsts>(
                          nullptr, "csrrci", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrs", atlas::Action::createAction<&RvzicsrInsts::csrrs_64_handler, RvzicsrInsts>(
                         nullptr, "csrrs", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrsi", atlas::Action::createAction<&RvzicsrInsts::csrrsi_64_handler, RvzicsrInsts>(
                          nullptr, "csrrsi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrw", atlas::Action::createAction<&RvzicsrInsts::csrrw_64_handler, RvzicsrInsts>(
                         nullptr, "csrrw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrwi", atlas::Action::createAction<&RvzicsrInsts::csrrwi_64_handler, RvzicsrInsts>(
                          nullptr, "csrrwi", ActionTags::EXECUTE_TAG));
    }

    ActionGroup* RvzicsrInsts::csrrs_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const auto & rs1 = insn->getRs1();
        const auto rs1_val = rs1->dmiRead<uint64_t>();
        const auto write = rs1_val != 0;

        const int csr = insn->getMavisOpcodeInfo()->getSpecialField(
            mavis::OpcodeInfo::SpecialField::CSR);

        auto reg = state->getCsrRegister(csr);
        if (!reg) {
            THROW_ILLEGAL_INSTRUCTION;
        }

        const reg_t old = reg->dmiRead<uint64_t>();

        if (write) {
            reg->write(old | rs1_val); //dmiWrite?
        }

        const auto rd_val = sext_xlen(old);
        insn->getRd()->write(rd_val); //dmiWrite?

        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrc_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const auto & rs1 = insn->getRs1();
        const auto rs1_val = rs1->dmiRead<uint64_t>();
        const auto write = rs1_val != 0;

        const int csr = insn->getMavisOpcodeInfo()->getSpecialField(
            mavis::OpcodeInfo::SpecialField::CSR);

        auto reg = state->getCsrRegister(csr);
        if (!reg) {
            THROW_ILLEGAL_INSTRUCTION;
        }

        const reg_t old = reg->dmiRead<uint64_t>();
        if (write) {
            reg->write(old & ~rs1_val); //dmiWrite?
        }

        const auto rd_val = sext_xlen(old);
        insn->getRd()->write(rd_val); //dmiWrite?

        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrwi_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int csr = insn->getMavisOpcodeInfo()->getSpecialField(
            mavis::OpcodeInfo::SpecialField::CSR);

        auto reg = state->getCsrRegister(csr);
        if (!reg) {
            THROW_ILLEGAL_INSTRUCTION;
        }

        const reg_t old = reg->dmiRead<uint64_t>();
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, 5>();
        reg->write(imm); //dmiWrite?

        const auto rd_val = sext_xlen(old);
        insn->getRd()->write(rd_val); //dmiWrite?

        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int csr = insn->getMavisOpcodeInfo()->getSpecialField(
            mavis::OpcodeInfo::SpecialField::CSR);

        const auto & rs1 = insn->getRs1();
        const auto rs1_val = rs1->dmiRead<uint64_t>();

        auto reg = state->getCsrRegister(csr);
        if (!reg) {
            THROW_ILLEGAL_INSTRUCTION;
        }

        const reg_t old = reg->dmiRead<uint64_t>();
        reg->write(old | rs1_val); //dmiWrite?

        const auto rd_val = sext_xlen(old);
        insn->getRd()->write(rd_val); //dmiWrite?

        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrsi_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int csr = insn->getMavisOpcodeInfo()->getSpecialField(
            mavis::OpcodeInfo::SpecialField::CSR);

        auto reg = state->getCsrRegister(csr);
        if (!reg) {
            THROW_ILLEGAL_INSTRUCTION;
        }

        const reg_t old = reg->dmiRead<uint64_t>();
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, 5>();
        if (imm) {
            reg->write(old | imm); //dmiWrite?
        }

        const auto rd_val = sext_xlen(old);
        insn->getRd()->write(rd_val); //dmiWrite?

        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrci_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int csr = insn->getMavisOpcodeInfo()->getSpecialField(
            mavis::OpcodeInfo::SpecialField::CSR);

        auto reg = state->getCsrRegister(csr);
        if (!reg) {
            THROW_ILLEGAL_INSTRUCTION;
        }

        const reg_t old = reg->dmiRead<uint64_t>();
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, 5>();
        if (imm) {
            reg->write(old & ~imm); //dmiWrite?
        }

        const auto rd_val = sext_xlen(old);
        insn->getRd()->write(rd_val); //dmiWrite?

        return nullptr;
    }

} // namespace atlas
