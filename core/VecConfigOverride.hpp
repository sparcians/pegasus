#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "mavis/JSONUtils.hpp"
#include "core/VecConfig.hpp"


namespace pegasus
{
    enum class VecCfgOverride : uint16_t
    {
        NO_OVERRIDE,

        EEW8  = 1 << 0,
        EEW16 = 1 << 1,
        EEW32 = 1 << 2,
        EEW64 = 1 << 3,

        EMUL1 = 1 << 4,
        EMUL2 = 1 << 5,
        EMUL4 = 1 << 6,
        EMUL8 = 1 << 7,

        VLMAX = 1 << 15,
    }; // class VecCfgOverride

    using VecCfgOverrides = std::vector<VecCfgOverride>;

    inline VecCfgOverride strToVecCfg(const std::string & str)
    {
        if (str == "eew8") return VecCfgOverride::EEW8;
        if (str == "eew16") return VecCfgOverride::EEW16;
        if (str == "eew32") return VecCfgOverride::EEW32;
        if (str == "eew64") return VecCfgOverride::EEW64;

        if (str == "emul1") return VecCfgOverride::EMUL1;
        if (str == "emul2") return VecCfgOverride::EMUL2;
        if (str == "emul4") return VecCfgOverride::EMUL4;
        if (str == "emul8") return VecCfgOverride::EMUL8;

        if (str == "vlmax") return VecCfgOverride::VLMAX;

        sparta_assert(false, "invalid veccfg string " << str);
    }

    inline VectorConfig makeVecCfg(const VectorConfig & config, const VecCfgOverrides & overrides)
    {
        VectorConfig cfg{config};
        bool setVLMAX = false;
        for (const auto override : overrides)
        {
            switch (override)
            {
                case VecCfgOverride::EEW8:
                    cfg.setSEW(8);
                    break;
                case VecCfgOverride::EEW16:
                    cfg.setSEW(16);
                    break;
                case VecCfgOverride::EEW32:
                    cfg.setSEW(32);
                    break;
                case VecCfgOverride::EEW64:
                    cfg.setSEW(64);
                    break;

                case VecCfgOverride::EMUL1:
                    cfg.setLMUL(1 * 8);
                    break;
                case VecCfgOverride::EMUL2:
                    cfg.setLMUL(2 * 8);
                    break;
                case VecCfgOverride::EMUL4:
                    cfg.setLMUL(4 * 8);
                    break;
                case VecCfgOverride::EMUL8:
                    cfg.setLMUL(8 * 8);
                    break;

                case VecCfgOverride::VLMAX:
                    setVLMAX = true;
                    break;

                default:
                    break;
            }
        }

        if (setVLMAX) //VLMAX relies on SEW and LMUL
        {
            cfg.setVL(cfg.getVLMAX());
        }
        return cfg;
    }

    inline VecCfgOverride tag_invoke(const boost::json::value_to_tag<VecCfgOverride> &, const boost::json::value& jv)
    {
        return strToVecCfg(jv.as_string().c_str());
    }
} // namespace pegasus
