#include "core/inst_handlers/b/RvzbbInsts.hpp"
#include "core/inst_handlers/b/RvbFunctors.hpp"
#include "core/inst_handlers/inst_helpers.hpp"
#include "include/PegasusUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "system/PegasusSystem.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzbbInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "andn",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Andn<uint64_t>>,
                                              RvzbbInsts>(nullptr, "andn",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clz", pegasus::Action::createAction<
                           &RvzbbInsts::unaryOpHandler_<RV64, CountlZero<uint64_t>>, RvzbbInsts>(
                           nullptr, "clz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clzw", pegasus::Action::createAction<
                            &RvzbbInsts::unaryOpHandler_<RV64, CountlZero<uint32_t>>, RvzbbInsts>(
                            nullptr, "clzw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cpop", pegasus::Action::createAction<
                            &RvzbbInsts::unaryOpHandler_<RV64, Popcount<uint64_t>>, RvzbbInsts>(
                            nullptr, "cpop", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cpopw", pegasus::Action::createAction<
                             &RvzbbInsts::unaryOpHandler_<RV64, Popcount<uint32_t>>, RvzbbInsts>(
                             nullptr, "cpopw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ctz", pegasus::Action::createAction<
                           &RvzbbInsts::unaryOpHandler_<RV64, CountrZero<uint64_t>>, RvzbbInsts>(
                           nullptr, "ctz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ctzw", pegasus::Action::createAction<
                            &RvzbbInsts::unaryOpHandler_<RV64, CountrZero<uint32_t>>, RvzbbInsts>(
                            nullptr, "ctzw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "max",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Max<int64_t>>,
                                              RvzbbInsts>(nullptr, "max", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "maxu",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Max<uint64_t>>,
                                              RvzbbInsts>(nullptr, "maxu",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "min",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Min<int64_t>>,
                                              RvzbbInsts>(nullptr, "min", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "minu",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Min<uint64_t>>,
                                              RvzbbInsts>(nullptr, "minu",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orc.b",
                pegasus::Action::createAction<&RvzbbInsts::orc_bHandler_<RV64>, RvzbbInsts>(
                    nullptr, "orc.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orn",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Orn<uint64_t>>,
                                              RvzbbInsts>(nullptr, "orn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rev8", pegasus::Action::createAction<
                            &RvzbbInsts::unaryOpHandler_<RV64, ByteSwap<uint64_t>>, RvzbbInsts>(
                            nullptr, "rev8", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rol",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Rol<uint64_t>>,
                                              RvzbbInsts>(nullptr, "rol", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rolw",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Rolw<int64_t>>,
                                              RvzbbInsts>(nullptr, "rolw",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ror",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Ror<uint64_t>>,
                                              RvzbbInsts>(nullptr, "ror", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rori",
                pegasus::Action::createAction<&RvzbbInsts::immOpHandler_<RV64, Ror<uint64_t>>,
                                              RvzbbInsts>(nullptr, "rori",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "roriw",
                pegasus::Action::createAction<&RvzbbInsts::immOpHandler_<RV64, Rorw<int64_t>>,
                                              RvzbbInsts>(nullptr, "roriw",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rorw",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Rorw<int64_t>>,
                                              RvzbbInsts>(nullptr, "rorw",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sext.b", pegasus::Action::createAction<
                              &RvzbbInsts::unaryOpHandler_<RV64, SextX<uint64_t, 8>>, RvzbbInsts>(
                              nullptr, "sext.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sext.h", pegasus::Action::createAction<
                              &RvzbbInsts::unaryOpHandler_<RV64, SextX<uint64_t, 16>>, RvzbbInsts>(
                              nullptr, "sext.h", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xnor",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Xnor<uint64_t>>,
                                              RvzbbInsts>(nullptr, "xnor",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "zext.h",
                pegasus::Action::createAction<&RvzbbInsts::unaryOpHandler_<RV64, ZextH<uint64_t>>,
                                              RvzbbInsts>(nullptr, "zext.h",
                                                          ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "andn",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV32, Andn<uint32_t>>,
                                              RvzbbInsts>(nullptr, "andn",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "clz", pegasus::Action::createAction<
                           &RvzbbInsts::unaryOpHandler_<RV32, CountlZero<uint32_t>>, RvzbbInsts>(
                           nullptr, "clz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cpop", pegasus::Action::createAction<
                            &RvzbbInsts::unaryOpHandler_<RV32, Popcount<uint32_t>>, RvzbbInsts>(
                            nullptr, "cpop", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ctz", pegasus::Action::createAction<
                           &RvzbbInsts::unaryOpHandler_<RV32, CountrZero<uint32_t>>, RvzbbInsts>(
                           nullptr, "ctz", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "max",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV32, Max<int32_t>>,
                                              RvzbbInsts>(nullptr, "max", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "maxu",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV32, Max<uint32_t>>,
                                              RvzbbInsts>(nullptr, "maxu",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "min",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV32, Min<int32_t>>,
                                              RvzbbInsts>(nullptr, "min", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "minu",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV32, Min<uint32_t>>,
                                              RvzbbInsts>(nullptr, "minu",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orc.b",
                pegasus::Action::createAction<&RvzbbInsts::orc_bHandler_<RV32>, RvzbbInsts>(
                    nullptr, "orc.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "orn",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV32, Orn<uint32_t>>,
                                              RvzbbInsts>(nullptr, "orn", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rev8", pegasus::Action::createAction<
                            &RvzbbInsts::unaryOpHandler_<RV32, ByteSwap<uint32_t>>, RvzbbInsts>(
                            nullptr, "rev8", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rol",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV32, Rol<uint32_t>>,
                                              RvzbbInsts>(nullptr, "rol", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ror",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV32, Ror<uint32_t>>,
                                              RvzbbInsts>(nullptr, "ror", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rori",
                pegasus::Action::createAction<&RvzbbInsts::immOpHandler_<RV32, Ror<uint32_t>>,
                                              RvzbbInsts>(nullptr, "rori",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sext.b", pegasus::Action::createAction<
                              &RvzbbInsts::unaryOpHandler_<RV32, SextX<uint32_t, 8>>, RvzbbInsts>(
                              nullptr, "sext.b", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sext.h", pegasus::Action::createAction<
                              &RvzbbInsts::unaryOpHandler_<RV32, SextX<uint32_t, 16>>, RvzbbInsts>(
                              nullptr, "sext.h", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "xnor",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV32, Xnor<uint32_t>>,
                                              RvzbbInsts>(nullptr, "xnor",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "zext.h",
                pegasus::Action::createAction<&RvzbbInsts::unaryOpHandler_<RV32, ZextH<uint32_t>>,
                                              RvzbbInsts>(nullptr, "zext.h",
                                                          ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzbbInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbbInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, typename OP>
    Action::ItrType RvzbbInsts::unaryOpHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        const XLEN rd_val = OP()(rs1_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN, typename OP>
    Action::ItrType RvzbbInsts::binaryOpHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        const XLEN rd_val = OP()(rs1_val, rs2_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN, typename OP>
    Action::ItrType RvzbbInsts::immOpHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        const XLEN imm_val = inst->getImmediate();

        const XLEN rd_val = OP()(rs1_val, imm_val);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rd_val);

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvzbbInsts::orc_bHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());

        XLEN output = 0;
        for (uint32_t i = 0; i < sizeof(XLEN); i++)
        {
            uint64_t byte_mask = 0xFFull << (i * 8);
            output |= byte_mask & -!!(rs1_val & byte_mask);
        }

        WRITE_INT_REG<XLEN>(state, inst->getRd(), output);

        return ++action_it;
    }
} // namespace pegasus
