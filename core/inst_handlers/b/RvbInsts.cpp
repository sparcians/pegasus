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
                "add.uw", atlas::Action::createAction<&RvbInsts::add_uw_handler, RvbInsts>(
                            nullptr, "add.uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh1add", atlas::Action::createAction<&RvbInsts::sh1add_handler<RV64>, RvbInsts>(
                            nullptr, "sh1add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh1add.uw", atlas::Action::createAction<&RvbInsts::sh1add_uw_handler, RvbInsts>(
                        nullptr, "sh1add.uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh2add", atlas::Action::createAction<&RvbInsts::sh2add_handler<RV64>, RvbInsts>(
                        nullptr, "sh2add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh2add.uw", atlas::Action::createAction<&RvbInsts::sh2add_uw_handler, RvbInsts>(
                        nullptr, "sh2add.uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh3add", atlas::Action::createAction<&RvbInsts::sh3add_handler<RV64>, RvbInsts>(
                        nullptr, "sh3add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sh3add.uw", atlas::Action::createAction<&RvbInsts::sh3add_uw_handler, RvbInsts>(
                        nullptr, "sh3add.uw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slli.uw", atlas::Action::createAction<&RvbInsts::slliuw_handler, RvbInsts>(
                        nullptr, "slli.uw", ActionTags::EXECUTE_TAG));
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

    Action::ItrType RvbInsts::add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const uint64_t rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const uint64_t rd_val = rs1_val + ((uint64_t)(uint32_t)(rs2_val));
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> Action::ItrType RvbInsts::sh1add_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << 1) + rs2_val;
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvbInsts::sh1add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << 1) + ((uint64_t)(uint32_t)(rs2_val));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> Action::ItrType RvbInsts::sh2add_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << 2) + rs2_val;
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvbInsts::sh2add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << 2) + ((uint64_t)(uint32_t)(rs2_val));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN> Action::ItrType RvbInsts::sh3add_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << 3) + rs2_val;
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvbInsts::sh3add_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<uint64_t>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<uint64_t>(state, inst->getRs2());

        const XLEN rd_val = (rs1_val << 3) + ((uint64_t)(uint32_t)(rs2_val));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    Action::ItrType RvbInsts::slli_uw_handler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const uint64_t rs1_val = static_cast<uint64_t>(static_cast<uint32_t>(READ_INT_REG<uint64_t>(state, insn->getRs1())));

        const uint32_t shift_amount = ((uint64_t)(uint32_t)(inst->getImmediate()));

        const uint64_t rd_val = rs1_val << shift_amount;
        WRITE_INT_REG<uint64_t>(state, inst->getRd(), rd_val);

        return ++action_it;
    }


    

    
}