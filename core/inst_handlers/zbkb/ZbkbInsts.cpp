#include "core/inst_handlers/zbkb/ZbkbInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/Trap.hpp"
#include "include/ActionTags.hpp"
#include "include/IntNums.hpp"

namespace pegasus
{

    template <typename XLEN>
    void ZbkbInsts::getInstHandlers(InstHandlers::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // Common instructions (RV32 + RV64)

        inst_handlers.emplace("andn",
                              Action::createAction<&ZbkbInsts::andnHandler_<XLEN>, ZbkbInsts>(
                                  nullptr, "andn", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("orn", Action::createAction<&ZbkbInsts::ornHandler_<XLEN>, ZbkbInsts>(
                                         nullptr, "orn", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("xnor",
                              Action::createAction<&ZbkbInsts::xnorHandler_<XLEN>, ZbkbInsts>(
                                  nullptr, "xnor", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("rol", Action::createAction<&ZbkbInsts::rolHandler_<XLEN>, ZbkbInsts>(
                                         nullptr, "rol", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("ror", Action::createAction<&ZbkbInsts::rorHandler_<XLEN>, ZbkbInsts>(
                                         nullptr, "ror", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("rori",
                              Action::createAction<&ZbkbInsts::roriHandler_<XLEN>, ZbkbInsts>(
                                  nullptr, "rori", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("rev8",
                              Action::createAction<&ZbkbInsts::rev8Handler_<XLEN>, ZbkbInsts>(
                                  nullptr, "rev8", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("brev8",
                              Action::createAction<&ZbkbInsts::brev8Handler_<XLEN>, ZbkbInsts>(
                                  nullptr, "brev8", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("pack",
                              Action::createAction<&ZbkbInsts::packHandler_<XLEN>, ZbkbInsts>(
                                  nullptr, "pack", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace("packh",
                              Action::createAction<&ZbkbInsts::packhHandler_<XLEN>, ZbkbInsts>(
                                  nullptr, "packh", ActionTags::EXECUTE_TAG));

        // RV64-only instructions
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace("packw",
                                  Action::createAction<&ZbkbInsts::packwHandler_<XLEN>, ZbkbInsts>(
                                      nullptr, "packw", ActionTags::EXECUTE_TAG));

            inst_handlers.emplace("rolw",
                                  Action::createAction<&ZbkbInsts::rolwHandler_<XLEN>, ZbkbInsts>(
                                      nullptr, "rolw", ActionTags::EXECUTE_TAG));

            inst_handlers.emplace("rorw",
                                  Action::createAction<&ZbkbInsts::rorwHandler_<XLEN>, ZbkbInsts>(
                                      nullptr, "rorw", ActionTags::EXECUTE_TAG));

            inst_handlers.emplace("roriw",
                                  Action::createAction<&ZbkbInsts::roriwHandler_<XLEN>, ZbkbInsts>(
                                      nullptr, "roriw", ActionTags::EXECUTE_TAG));
        }

        // RV32-only instructions
        if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace("zip",
                                  Action::createAction<&ZbkbInsts::zipHandler_<XLEN>, ZbkbInsts>(
                                      nullptr, "zip", ActionTags::EXECUTE_TAG));

            inst_handlers.emplace("unzip",
                                  Action::createAction<&ZbkbInsts::unzipHandler_<XLEN>, ZbkbInsts>(
                                      nullptr, "unzip", ActionTags::EXECUTE_TAG));
        }
    }

    template void ZbkbInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void ZbkbInsts::getInstHandlers<RV64>(std::map<std::string, Action>&);

    template <typename XLEN>
    Action::ItrType ZbkbInsts::andnHandler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN rs2 = READ_INT_REG<XLEN>(state, inst->getRs2());
        XLEN result = rs1 & ~rs2;
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::ornHandler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN rs2 = READ_INT_REG<XLEN>(state, inst->getRs2());
        XLEN result = rs1 | ~rs2;
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::xnorHandler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN rs2 = READ_INT_REG<XLEN>(state, inst->getRs2());
        XLEN result = ~(rs1 ^ rs2);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::rolHandler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN rs2 = READ_INT_REG<XLEN>(state, inst->getRs2());
        XLEN shift = rs2 & (sizeof(XLEN) * 8 - 1);
        XLEN result = rol_(rs1, shift);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::rorHandler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN rs2 = READ_INT_REG<XLEN>(state, inst->getRs2());
        XLEN shift = rs2 & (sizeof(XLEN) * 8 - 1);
        XLEN result = ror_(rs1, shift);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::roriHandler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN shamt = inst->getImmediate();

        if constexpr (std::is_same_v<XLEN, RV32>)
        {
            if (shamt & 0x20)
                THROW_ILLEGAL_INST;
        }

        XLEN result = ror_(rs1, shamt);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::rev8Handler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN result = rev8_(rs1);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::brev8Handler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN result = brev8_(rs1);
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::packHandler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN rs2 = READ_INT_REG<XLEN>(state, inst->getRs2());
        XLEN result;

        if constexpr (sizeof(XLEN) == 8)
            result = (rs1 & 0xFFFFFFFFULL) | ((rs2 & 0xFFFFFFFFULL) << 32);
        else
            result = (rs1 & 0xFFFF) | ((rs2 & 0xFFFF) << 16);

        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::packhHandler_(PegasusState* state, Action::ItrType itr)
    {
        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN rs2 = READ_INT_REG<XLEN>(state, inst->getRs2());

        uint16_t packed = (rs1 & 0xFF) | ((rs2 & 0xFF) << 8);
        XLEN result = static_cast<XLEN>(packed);

        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::packwHandler_(PegasusState* state, Action::ItrType itr)
    {
        if constexpr (std::is_same_v<XLEN, RV32>)
            THROW_ILLEGAL_INST;

        const auto & inst = state->getCurrentInst();
        XLEN rs1 = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN rs2 = READ_INT_REG<XLEN>(state, inst->getRs2());

        XLEN result = (rs1 & 0xFFFF) | ((rs2 & 0xFFFF) << 16);
        result &= 0xFFFFFFFFULL;

        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::rolwHandler_(PegasusState* state, Action::ItrType itr)
    {
        if constexpr (std::is_same_v<XLEN, RV32>)
            THROW_ILLEGAL_INST;

        const auto & inst = state->getCurrentInst();
        uint32_t rs1_low = static_cast<uint32_t>(READ_INT_REG<XLEN>(state, inst->getRs1()));
        uint32_t rs2_low = static_cast<uint32_t>(READ_INT_REG<XLEN>(state, inst->getRs2()));

        uint32_t shift = rs2_low & 0x1F;
        uint32_t result32 = rol_(rs1_low, shift);

        XLEN result = static_cast<XLEN>(static_cast<int32_t>(result32));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::rorwHandler_(PegasusState* state, Action::ItrType itr)
    {
        if constexpr (std::is_same_v<XLEN, RV32>)
            THROW_ILLEGAL_INST;

        const auto & inst = state->getCurrentInst();
        uint32_t rs1_low = static_cast<uint32_t>(READ_INT_REG<XLEN>(state, inst->getRs1()));
        uint32_t rs2_low = static_cast<uint32_t>(READ_INT_REG<XLEN>(state, inst->getRs2()));

        uint32_t shift = rs2_low & 0x1F;
        uint32_t result32 = ror_(rs1_low, shift);

        XLEN result = static_cast<XLEN>(static_cast<int32_t>(result32));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::roriwHandler_(PegasusState* state, Action::ItrType itr)
    {
        if constexpr (std::is_same_v<XLEN, RV32>)
            THROW_ILLEGAL_INST;

        const auto & inst = state->getCurrentInst();
        uint32_t rs1_low = static_cast<uint32_t>(READ_INT_REG<XLEN>(state, inst->getRs1()));
        uint32_t shamt = inst->getImmediate() & 0x1F;

        uint32_t result32 = ror_(rs1_low, shamt);
        XLEN result = static_cast<XLEN>(static_cast<int32_t>(result32));

        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::zipHandler_(PegasusState* state, Action::ItrType itr)
    {
        if constexpr (std::is_same_v<XLEN, RV64>)
            THROW_ILLEGAL_INST;

        const auto & inst = state->getCurrentInst();
        XLEN val = READ_INT_REG<XLEN>(state, inst->getRs1());
        XLEN result = 0;

        for (int i = 0; i < 16; ++i)
        {
            result |= ((val >> i) & 1) << (2 * i);
            result |= ((val >> (i + 16)) & 1) << (2 * i + 1);
        }

        WRITE_INT_REG<XLEN>(state, inst->getRd(), result);
        return ++itr;
    }

    template <typename XLEN>
    Action::ItrType ZbkbInsts::unzipHandler_(PegasusState* state, Action::ItrType itr)
    {
        if constexpr (std::is_same_v<XLEN, RV64>)
            THROW_ILLEGAL_INST;

        const auto & inst = state->getCurrentInst();
        uint32_t val = static_cast<uint32_t>(READ_INT_REG<XLEN>(state, inst->getRs1()));
        uint32_t result = 0;

        for (int i = 0; i < 16; ++i)
        {
            result |= ((val >> (2 * i)) & 1) << i;
            result |= ((val >> (2 * i + 1)) & 1) << (i + 16);
        }

        WRITE_INT_REG<XLEN>(state, inst->getRd(), static_cast<XLEN>(result));
        return ++itr;
    }


} // namespace pegasus