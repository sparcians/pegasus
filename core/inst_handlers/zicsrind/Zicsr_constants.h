#pragma once

namespace pegasus
{

    // CSRIND extension register types
    enum class Csrind_regType
    {
        CLICINTCTL,
        CLICINTATTR,
        CLICINTIP,
        CLICINTIE,
        CLICINTTRIG,
        CLICCFG,
        INVALID
    };

    const uint32_t CLICINT_CTL_ATTR_LO  = 0x1000;
    const uint32_t CLICINT_CTL_ATTR_HI  = 0x13ff;
    const uint32_t CLICINT_IP_IE_LO     = 0x1400;
    const uint32_t CLICINT_IP_IE_HI     = 0x137f;
    const uint32_t CLICINT_TRIG_LO      = 0x1480;
    const uint32_t CLICINT_TRIG_HI      = 0x139f;
    const uint32_t CLICINT_CFG_LO       = 0x14A0;
    const uint32_t CLICINT_CFG_HI       = 0x13A0;

    constexpr bool IS_CLICINT_CTL_ATTR(const uint32_t xisel)
    {
        return xisel >= CLICINT_CTL_ATTR_LO && xisel <= CLICINT_CTL_ATTR_HI;
    }
    constexpr bool IS_CLICINT_IP_IE(const uint32_t xisel)
    {
        return xisel >= CLICINT_IP_IE_LO && xisel <= CLICINT_IP_IE_HI;
    }
    constexpr bool IS_CLICINT_TRIG(const uint32_t xisel)
    {
        return xisel >= CLICINT_TRIG_LO && xisel <= CLICINT_TRIG_HI;
    }
    constexpr bool IS_CLICINT_CFG(const uint32_t xisel)
    {
        return xisel == CLICINT_CTL_ATTR_LO;
    }
}
