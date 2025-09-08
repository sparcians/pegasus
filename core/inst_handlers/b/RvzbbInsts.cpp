#include "core/inst_handlers/b/RvzbbInsts.hpp"
#include "core/inst_handlers/b/RvbFunctors.hpp"
#include "core/inst_handlers/i/RviFunctors.hpp"
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
        using SXLEN = std::make_signed_t<XLEN>;

        inst_handlers.emplace(
            "andn",
            pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<XLEN, Andn<XLEN>>,
                                          RvzbInstsBase>(nullptr, "andn",
                                                      ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "clz", pegasus::Action::createAction<
                       &RvzbbInsts::unaryOpHandler_<XLEN, CountlZero<XLEN>>, RvzbInstsBase>(
                       nullptr, "clz", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "cpop", pegasus::Action::createAction<
                        &RvzbbInsts::unaryOpHandler_<XLEN, Popcount<XLEN>>, RvzbInstsBase>(
                        nullptr, "cpop", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "ctz", pegasus::Action::createAction<
                       &RvzbbInsts::unaryOpHandler_<XLEN, CountrZero<XLEN>>, RvzbInstsBase>(
                       nullptr, "ctz", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "max",
            pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Max<SXLEN>>,
                                          RvzbInstsBase>(nullptr, "max", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "maxu",
            pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<XLEN, Max<XLEN>>,
                                          RvzbInstsBase>(nullptr, "maxu",
                                                      ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "min",
            pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Min<SXLEN>>,
                                          RvzbInstsBase>(nullptr, "min", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "minu",
            pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<XLEN, Min<XLEN>>,
                                          RvzbInstsBase>(nullptr, "minu",
                                                      ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "orc.b",
            pegasus::Action::createAction<&RvzbbInsts::orc_bHandler_<XLEN>, RvzbbInsts>(
                nullptr, "orc.b", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "orn",
            pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<XLEN, Orn<XLEN>>,
                                          RvzbInstsBase>(nullptr, "orn", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "rev8", pegasus::Action::createAction<
                        &RvzbbInsts::unaryOpHandler_<XLEN, ByteSwap<XLEN>>, RvzbInstsBase>(
                        nullptr, "rev8", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "rol",
            pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<XLEN, Rol<XLEN>>,
                                          RvzbInstsBase>(nullptr, "rol", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "ror",
            pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<XLEN, Ror<XLEN>>,
                                          RvzbInstsBase>(nullptr, "ror", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "rori",
            pegasus::Action::createAction<&RvzbbInsts::immOpHandler_<XLEN, Ror<XLEN>>,
                                          RvzbInstsBase>(nullptr, "rori",
                                                      ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sext.b", pegasus::Action::createAction<
                          &RvzbbInsts::unaryOpHandler_<XLEN, SextX<XLEN, 8>>, RvzbInstsBase>(
                          nullptr, "sext.b", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "sext.h", pegasus::Action::createAction<
                          &RvzbbInsts::unaryOpHandler_<XLEN, SextX<XLEN, 16>>, RvzbInstsBase>(
                          nullptr, "sext.h", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "xnor",
            pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<XLEN, Xnor<XLEN>>,
                                          RvzbInstsBase>(nullptr, "xnor",
                                                      ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "zext.h",
            pegasus::Action::createAction<&RvzbbInsts::unaryOpHandler_<XLEN, ZextH<XLEN>>,
                                          RvzbInstsBase>(nullptr, "zext.h",
                                                      ActionTags::EXECUTE_TAG));

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "clzw", pegasus::Action::createAction<
                            &RvzbbInsts::unaryOpHandler_<RV64, CountlZero<uint32_t>>, RvzbInstsBase>(
                            nullptr, "clzw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cpopw", pegasus::Action::createAction<
                             &RvzbbInsts::unaryOpHandler_<RV64, Popcount<uint32_t>>, RvzbInstsBase>(
                             nullptr, "cpopw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ctzw", pegasus::Action::createAction<
                            &RvzbbInsts::unaryOpHandler_<RV64, CountrZero<uint32_t>>, RvzbInstsBase>(
                            nullptr, "ctzw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rolw",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Rolw<int64_t>>,
                                              RvzbInstsBase>(nullptr, "rolw",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "roriw",
                pegasus::Action::createAction<&RvzbbInsts::immOpHandler_<RV64, Rorw<int64_t>>,
                                              RvzbInstsBase>(nullptr, "roriw",
                                                          ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "rorw",
                pegasus::Action::createAction<&RvzbbInsts::binaryOpHandler_<RV64, Rorw<int64_t>>,
                                              RvzbInstsBase>(nullptr, "rorw",
                                                          ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzbbInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzbbInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

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
