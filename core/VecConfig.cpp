#include "core/VecConfig.hpp"
#include "core/PegasusState.hpp"
#include "sparta/utils/SpartaTester.hpp"

namespace pegasus
{
    template <typename XLEN> XLEN VectorConfig::vsetAVL(PegasusState* state, bool set_max, XLEN avl)
    {
        const size_t vl = set_max ? getVLMAX() : std::min<size_t>(getVLMAX(), avl);
        setVL(vl);
        WRITE_CSR_REG<XLEN>(state, VL, vl);
        return vl;
    }

    template <typename XLEN> void VectorConfig::vsetVTYPE(PegasusState* state, XLEN vtype)
    {
        WRITE_CSR_REG<XLEN>(state, VTYPE, vtype);
        const size_t vlmul = READ_CSR_FIELD<XLEN>(state, VTYPE, "vlmul");

        static const size_t lmul_table[8] = {
            8,  // 000
            16, // 001
            32, // 010
            64, // 011
            0,  // 100 (invalid)
            1,  // 101
            2,  // 110
            4   // 111
        };

        const size_t lmul = lmul_table[vlmul & 0b111];
        sparta_assert(lmul, "Invalid vtype VLMUL encoding.");
        setLMUL(lmul);
        setSEW(8u << READ_CSR_FIELD<XLEN>(state, VTYPE, "vsew"));
        setVTA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vta"));
        setVMA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vma"));
    }

    template RV32 VectorConfig::vsetAVL<RV32>(PegasusState*, bool, RV32);
    template RV64 VectorConfig::vsetAVL<RV64>(PegasusState*, bool, RV64);
    template void VectorConfig::vsetVTYPE<RV32>(PegasusState*, RV32);
    template void VectorConfig::vsetVTYPE<RV64>(PegasusState*, RV64);

    template <bool IS_UNIT_TEST> bool VectorConfig::compare(const VectorConfig* config) const
    {
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(vlen_, config->vlen_);
            EXPECT_EQUAL(lmul_, config->lmul_);
            EXPECT_EQUAL(sew_, config->sew_);
            EXPECT_EQUAL(vta_, config->vta_);
            EXPECT_EQUAL(vma_, config->vma_);
            EXPECT_EQUAL(vl_, config->vl_);
            EXPECT_EQUAL(vstart_, config->vstart_);
        }
        else
        {
            if (vlen_ != config->vlen_)
            {
                return false;
            }
            if (lmul_ != config->lmul_)
            {
                return false;
            }
            if (sew_ != config->sew_)
            {
                return false;
            }
            if (vta_ != config->vta_)
            {
                return false;
            }
            if (vma_ != config->vma_)
            {
                return false;
            }
            if (vl_ != config->vl_)
            {
                return false;
            }
            if (vstart_ != config->vstart_)
            {
                return false;
            }
        }

        return true;
    }

    template bool VectorConfig::compare<false>(const VectorConfig* config) const;
    template bool VectorConfig::compare<true>(const VectorConfig* config) const;

} // namespace pegasus
