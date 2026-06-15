#include "core/VectorConfig.hpp"
#include "core/PegasusState.hpp"
#include "sparta/utils/SpartaTester.hpp"

namespace pegasus
{
    /**
     * @brief Compares the current VectorConfig object with another VectorConfig object.
     *
     * This function compares the fields of the current VectorConfig object with those of
     * another VectorConfig object. If `IS_UNIT_TEST` is `true`, the comparison uses unit
     * test assertions to validate equality. Otherwise, it performs a field-by-field
     * comparison and returns `true` if all fields are equal, or `false` otherwise.
     *
     * @tparam IS_UNIT_TEST A boolean indicating whether the function is being used in a unit test
     * context. If `true`, unit test assertions are used for comparison.
     * @param config Pointer to the VectorConfig object to compare against.
     * @return `true` if all fields are equal (when `IS_UNIT_TEST` is `false`), otherwise `false`.
     */
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
