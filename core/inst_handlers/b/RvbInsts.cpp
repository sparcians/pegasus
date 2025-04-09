#include "core/inst_handlers/b/RvbInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/AtlasUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvbInsts::getInstHandlers(Execute::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "add.uw", atlas::Action::createAction<&RvbInsts::add_uw_handler<RV64>, RvbInsts>(
                            nullptr, "add.uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh1add", atlas::Action::createAction<&RvbInsts::sh1add_handler<RV64>, RvbInsts>(
                            nullptr, "sh1add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh1add_uw", atlas::Action::createAction<&RvbInsts::sh1add_uw_handler, RvbInsts>(
                        nullptr, "sh1add_uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh2add", atlas::Action::createAction<&RvbInsts::sh2add_handler<RV64>, RvbInsts>(
                        nullptr, "sh2add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh2add_uw", atlas::Action::createAction<&RvbInsts::sh2add_uw_handler, RvbInsts>(
                        nullptr, "sh2add_uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh3add", atlas::Action::createAction<&RvbInsts::sh3add_handler<RV64>, RvbInsts>(
                        nullptr, "sh3add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh3add_uw", atlas::Action::createAction<&RvbInsts::sh3add_uw_handler, RvbInsts>(
                        nullptr, "sh3add_uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slliuw", atlas::Action::createAction<&RvbInsts::slliuw_handler, RvbInsts>(
                        nullptr, "slliuw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "zextw", atlas::Action::createAction<&RvbInsts::zextw_handler, RvbInsts>(
                        nullptr, "zextw", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
        inst_handlers.emplace(
            "sh1add", atlas::Action::createAction<&RvbInsts::sh1add_handler<RV32>, RvbInsts>(
                nullptr, "sh1add", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sh2add", atlas::Action::createAction<&RvbInsts::sh2add_handler<RV32>, RvbInsts>(
                nullptr, "sh2add", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sh3add", atlas::Action::createAction<&RvbInsts::sh3add_handler<RV32>, RvbInsts>(
                nullptr, "sh3add", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvbInsts::getInstHandlers<RV32>(Execute::InstHandlersMap &);
    template void RvbInsts::getInstHandlers<RV64>(Execute::InstHandlersMap &);

    template<> ActionGroup* RvbInsts::add_uw_handler<RV64>(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, insn->getRs1());
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, insn->getRs2());

        const uint64_t rd_val = ((uint64_t)(uint32_t)(rs1_val)) + ((uint64_t)(uint32_t)(rs2_val));
        WRITE_INT_REG<uint64_t>(state, insn->getRd(), rd_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvbInsts::sh1add(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<uint64_t>(state, insn->getRs1());
        const XLEN rs2_val = READ_INT_REG<uint64_t>(state, insn->getRs2());

        const XLEN rd_val = (rs1_val << 1) + rs2_val;
        WRITE_INT_REG<XLEN>(state, insn->getRd(), rd_val);

        return nullptr;
    }
}