#include "arch/RegisterSet.hpp"
#include "core/Exception.hpp"
#include "core/Trap.hpp"
#include "core/inst_handlers/zicsrind/Rvzicsrind.hpp"
#include "include/gen/CSRNums.hpp"

namespace pegasus
{
    template <typename XLEN, uint32_t XISELECT, uint32_t XIIDX>
    CsrindRegType Rvzicsrind::validate_xiselect(PegasusState* state, int & offset)
    {
        const XLEN xiselect_val = READ_CSR_REG<XLEN>(state, XISELECT);

        // Try to match the xiselect value
        if (IS_CLICINT_CTL_ATTR(xiselect_val))
        {
            if constexpr (XIIDX == 1)
            {
                offset = xiselect_val - CLICINT_CTL_ATTR_LO;
                return CsrindRegType::CLICINTCTL;
            }
            if constexpr (XIIDX == 2)
            {
                offset = xiselect_val - CLICINT_CTL_ATTR_LO;
                return CsrindRegType::CLICINTATTR;
            }
        }
        else if (IS_CLICINT_IP_IE(xiselect_val))
        {
            if constexpr (XIIDX == 1)
            {
                offset = xiselect_val - CLICINT_IP_IE_LO;
                return CsrindRegType::CLICINTIP;
            }
            if constexpr (XIIDX == 2)
            {
                offset = xiselect_val - CLICINT_IP_IE_LO;
                return CsrindRegType::CLICINTIE;
            }
        }
        else if (IS_CLICINT_TRIG(xiselect_val))
        {
            if constexpr (XIIDX == 1)
            {
                offset = xiselect_val - CLICINT_TRIG_LO;
                return CsrindRegType::CLICINTTRIG;
            }
        }
        else if (IS_CLICINT_CFG(xiselect_val))
        {
            if constexpr (XIIDX == 1)
            {
                offset = 0;
                return CsrindRegType::CLICCFG;
            }
        }

        offset = -1;
        return CsrindRegType::INVALID;
    }

    // Install register callback functions
    // This macro registers a read/write callback function into the register
    // with name REGNAME, the helper functions must be of the form
    //   template <typename XLEN, int IDX> XLEN <HELPER>R(PegasusState* state,
    //   sparta::RegisterBase*) template <typename XLEN, int IDX> void <HELPER>W(PegasusState*
    //   state, sparta::RegisterBase*, XLEN)
#define INSTALL_REG_CB(REGNAME, HELPER, NUM)                                                       \
    do                                                                                             \
    {                                                                                              \
        auto REGNAME##_reg = state->findRegister(#REGNAME, false);                                 \
        if (REGNAME##_reg)                                                                         \
        {                                                                                          \
            REGNAME##_reg->addReadCB(                                                              \
                [state](sparta::RegisterBase* r)                                                   \
                {                                                                                  \
                    XLEN value = HELPER##R<XLEN, NUM>(state, r);                                   \
                    return sparta::utils::ValidValue<uint64_t>(static_cast<uint64_t>(value));      \
                });                                                                                \
                                                                                                   \
            REGNAME##_reg->addWriteCB(                                                             \
                [state](sparta::RegisterBase* r, uint64_t v)                                       \
                {                                                                                  \
                    HELPER##W<XLEN, NUM>(state, r, static_cast<XLEN>(v));                          \
                    return true;                                                                   \
                });                                                                                \
        }                                                                                          \
    } while (0)

    template <typename XLEN> void Rvzicsrind::addCSRRegisterCallbacks(PegasusState* state)
    {
        static_assert(sizeof(XLEN) == 4 || sizeof(XLEN) == 8);

        INSTALL_REG_CB(mireg, mireg, 1);
        INSTALL_REG_CB(mireg2, mireg, 2);
        INSTALL_REG_CB(mireg3, mireg, 3);
        INSTALL_REG_CB(mireg4, mireg, 4);
        INSTALL_REG_CB(mireg5, mireg, 5);
        INSTALL_REG_CB(mireg6, mireg, 6);
        INSTALL_REG_CB(sireg, sireg, 1);
        INSTALL_REG_CB(sireg2, sireg, 2);
        INSTALL_REG_CB(sireg3, sireg, 3);
        INSTALL_REG_CB(sireg4, sireg, 4);
        INSTALL_REG_CB(sireg5, sireg, 5);
        INSTALL_REG_CB(sireg6, sireg, 6);
    }

    template void Rvzicsrind::addCSRRegisterCallbacks<RV32>(PegasusState* state);
    template void Rvzicsrind::addCSRRegisterCallbacks<RV64>(PegasusState* state);

    template <typename XLEN, int XIIDX>
    XLEN Rvzicsrind::miregR(PegasusState* state, sparta::RegisterBase* reg)
    {
        int offset;
        auto reg_type = validate_xiselect<XLEN, MISELECT, XIIDX>(state, offset);

        if (reg_type == CsrindRegType::INVALID)
        {
            state->throwException(FaultCause::ILLEGAL_INST);
        }

        // Right now we dont have a clic model to use the decoded offset.
        // But when we do...
        // this value would be passed to the clic model.
        // Or whatever model needs this value to modify its internal state.
        (void)offset;

        return reg->read<XLEN>();
    }

    template <typename XLEN, int XIIDX>
    void Rvzicsrind::miregW(PegasusState* state, sparta::RegisterBase* reg, XLEN val)
    {
        int offset;
        auto reg_type = validate_xiselect<XLEN, MISELECT, XIIDX>(state, offset);

        if (reg_type == CsrindRegType::INVALID)
        {
            state->throwException(FaultCause::ILLEGAL_INST);
        }

        reg->write<XLEN>(val);
    }

    template <typename XLEN, int XIIDX>
    XLEN Rvzicsrind::siregR(PegasusState* state, sparta::RegisterBase* reg)
    {
        int offset;
        auto reg_type = validate_xiselect<XLEN, MISELECT, XIIDX>(state, offset);

        if (reg_type == CsrindRegType::INVALID)
        {
            state->throwException(FaultCause::ILLEGAL_INST);
        }

        return reg->read<XLEN>();
    }

    template <typename XLEN, int XIIDX>
    void Rvzicsrind::siregW(PegasusState* state, sparta::RegisterBase* reg, XLEN val)
    {
        int offset;
        auto reg_type = validate_xiselect<XLEN, MISELECT, XIIDX>(state, offset);

        if (reg_type == CsrindRegType::INVALID)
        {
            state->throwException(FaultCause::ILLEGAL_INST);
        }

        reg->write<XLEN>(val);
    }
} // namespace pegasus
