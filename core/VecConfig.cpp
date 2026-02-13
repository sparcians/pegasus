#include "core/VecConfig.hpp"
#include "core/PegasusState.hpp"
#include "sparta/utils/SpartaTester.hpp"

namespace pegasus
{
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
