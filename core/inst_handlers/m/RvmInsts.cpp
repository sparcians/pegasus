#include "core/inst_handlers/m/RvmInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvmInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same<XLEN, RV64>::value || std::is_same<XLEN, RV32>::value);
        if constexpr (std::is_same<XLEN, RV64>::value)
        {
            inst_handlers.emplace("div",
                                  atlas::Action::createAction<&RvmInsts::div_64_handler, RvmInsts>(
                                      nullptr, "div", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("divu",
                                  atlas::Action::createAction<&RvmInsts::divu_64_handler, RvmInsts>(
                                      nullptr, "divu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "divuw", atlas::Action::createAction<&RvmInsts::divuw_64_handler, RvmInsts>(
                             nullptr, "divuw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("divw",
                                  atlas::Action::createAction<&RvmInsts::divw_64_handler, RvmInsts>(
                                      nullptr, "divw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("mul",
                                  atlas::Action::createAction<&RvmInsts::mul_64_handler, RvmInsts>(
                                      nullptr, "mul", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("mulh",
                                  atlas::Action::createAction<&RvmInsts::mulh_64_handler, RvmInsts>(
                                      nullptr, "mulh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mulhsu", atlas::Action::createAction<&RvmInsts::mulhsu_64_handler, RvmInsts>(
                              nullptr, "mulhsu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "mulhu", atlas::Action::createAction<&RvmInsts::mulhu_64_handler, RvmInsts>(
                             nullptr, "mulhu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("mulw",
                                  atlas::Action::createAction<&RvmInsts::mulw_64_handler, RvmInsts>(
                                      nullptr, "mulw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("rem",
                                  atlas::Action::createAction<&RvmInsts::rem_64_handler, RvmInsts>(
                                      nullptr, "rem", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("remu",
                                  atlas::Action::createAction<&RvmInsts::remu_64_handler, RvmInsts>(
                                      nullptr, "remu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "remuw", atlas::Action::createAction<&RvmInsts::remuw_64_handler, RvmInsts>(
                             nullptr, "remuw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("remw",
                                  atlas::Action::createAction<&RvmInsts::remw_64_handler, RvmInsts>(
                                      nullptr, "remw", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same<XLEN, RV32>::value)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template void RvmInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvmInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    ActionGroup* RvmInsts::div_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        sreg_t lhs = sext_xlen(rs1_val);
        sreg_t rhs = sext_xlen(rs2_val);

        if (rhs == 0)
        {
            insn->getRd()->write(UINT64_MAX);
        }
        else if (lhs == INT64_MIN && rhs == -1)
        {
            insn->getRd()->write(lhs);
        }
        else
        {
            insn->getRd()->write(sext_xlen(lhs / rhs));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::divu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        reg_t lhs = zext_xlen(rs1_val);
        reg_t rhs = zext_xlen(rs2_val);
        if (rhs == 0)
        {
            insn->getRd()->write(UINT64_MAX);
        }
        else
        {
            insn->getRd()->write(sext_xlen(lhs / rhs));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::divuw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        reg_t lhs = zext32(rs1_val);
        reg_t rhs = zext32(rs2_val);
        if (rhs == 0)
        {
            insn->getRd()->write(UINT64_MAX);
        }
        else
        {
            insn->getRd()->write(sext32(lhs / rhs));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::divw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        sreg_t lhs = sext32(rs1_val);
        sreg_t rhs = sext32(rs2_val);
        if (rhs == 0)
        {
            insn->getRd()->write(UINT64_MAX);
        }
        else
        {
            insn->getRd()->write(sext32(lhs / rhs));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::mul_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();
        const uint64_t rd_val = rs1_val * rs2_val;
        insn->getRd()->write(rd_val);

        return nullptr;
    }

    ActionGroup* RvmInsts::mulh_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        if (xlen == 64)
        {
            insn->getRd()->write(mulh(rs1_val, rs2_val));
        }
        else
        {
            insn->getRd()->write(sext32((sext32(rs1_val) * sext32(rs2_val)) >> 32));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::mulhsu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        if (xlen == 64)
        {
            insn->getRd()->write(mulhsu(rs1_val, rs2_val));
        }
        else
        {
            insn->getRd()->write(sext32((sext32(rs1_val) * reg_t((uint32_t)rs2_val)) >> 32));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::mulhu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        if (xlen == 64)
        {
            insn->getRd()->write(mulhu(rs1_val, rs2_val));
        }
        else
        {
            insn->getRd()->write(
                sext32(((uint64_t)(uint32_t)rs1_val * (uint64_t)(uint32_t)rs2_val) >> 32));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::mulw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();
        insn->getRd()->write(sext32(rs1_val * rs2_val));

        return nullptr;
    }

    ActionGroup* RvmInsts::rem_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        sreg_t lhs = sext_xlen(rs1_val);
        sreg_t rhs = sext_xlen(rs2_val);

        if (rhs == 0)
        {
            insn->getRd()->write(lhs);
        }
        else if (lhs == INT64_MIN && rhs == -1)
        {
            insn->getRd()->write(0);
        }
        else
        {
            insn->getRd()->write(sext_xlen(lhs % rhs));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::remu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        reg_t lhs = zext_xlen(rs1_val);
        reg_t rhs = zext_xlen(rs2_val);
        if (rhs == 0)
        {
            insn->getRd()->write(sext_xlen(lhs));
        }
        else
        {
            insn->getRd()->write(sext_xlen(lhs % rhs));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::remuw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        reg_t lhs = zext32(rs1_val);
        reg_t rhs = zext32(rs2_val);
        if (rhs == 0)
        {
            insn->getRd()->write(sext32(lhs));
        }
        else
        {
            insn->getRd()->write(sext32(lhs % rhs));
        }

        return nullptr;
    }

    ActionGroup* RvmInsts::remw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->read();
        const uint64_t rs2_val = insn->getRs2()->read();

        sreg_t lhs = sext32(rs1_val);
        sreg_t rhs = sext32(rs2_val);
        if (rhs == 0)
        {
            insn->getRd()->write(lhs);
        }
        else
        {
            insn->getRd()->write(sext32(lhs % rhs));
        }

        return nullptr;
    }
} // namespace atlas
