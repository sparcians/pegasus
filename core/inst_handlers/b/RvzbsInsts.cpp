#include "core/inst_handlers/b/RvzbsInsts.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/AtlasUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "system/AtlasSystem.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvzbsInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "bclr",
            atlas::Action::createAction<&RvzbsInsts::binaryOpHandler<XLEN, RvzbsInsts::bclr<XLEN>>,
                                        RvzbsInsts>(nullptr, "bclr", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bclri",
            atlas::Action::createAction<&RvzbsInsts::immOpHandler<XLEN, RvzbsInsts::bclr<XLEN>>,
                                        RvzbsInsts>(nullptr, "bclri", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bext",
            atlas::Action::createAction<&RvzbsInsts::binaryOpHandler<XLEN, RvzbsInsts::bext<XLEN>>,
                                        RvzbsInsts>(nullptr, "bext", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bexti",
            atlas::Action::createAction<&RvzbsInsts::immOpHandler<XLEN, RvzbsInsts::bext<XLEN>>,
                                        RvzbsInsts>(nullptr, "bexti", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "binv",
            atlas::Action::createAction<&RvzbsInsts::binaryOpHandler<XLEN, RvzbsInsts::binv<XLEN>>,
                                        RvzbsInsts>(nullptr, "binv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "binvi",
            atlas::Action::createAction<&RvzbsInsts::immOpHandler<XLEN, RvzbsInsts::binv<XLEN>>,
                                        RvzbsInsts>(nullptr, "binvi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bset",
            atlas::Action::createAction<&RvzbsInsts::binaryOpHandler<XLEN, RvzbsInsts::bset<XLEN>>,
                                        RvzbsInsts>(nullptr, "bset", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "bseti",
            atlas::Action::createAction<&RvzbsInsts::immOpHandler<XLEN, RvzbsInsts::bset<XLEN>>,
                                        RvzbsInsts>(nullptr, "bseti", ActionTags::EXECUTE_TAG));
    }

    template void RvzbsInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbsInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, typename OP>
    Action::ItrType RvzbsInsts::binaryOpHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = OP()(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN, typename OP>
    Action::ItrType RvzbsInsts::immOpHandler(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        const XLEN rd_val = OP()(rs1_val, imm_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }
} // namespace atlas