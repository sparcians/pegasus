#pragma once

#include "sparta/utils/SpartaSharedPointerAllocator.hpp"
#include "include/CSRFieldIdxs64.hpp"

namespace atlas
{
    // TODO AtlasRegister: Determine if this class is still needed.
    //
    // The RISC-V spec has several implicit register behaviors (listed below). Need to determine
    // if this behavior should be handled in a register class like this one, or handled by each
    // instruction handler.
    //
    // 1. 32-bit instructions in RV64 (e.g. addw) operate on the lower 32-bits of the register but
    // write their results as a 64-bit sign-extended value into the 64-bit register.
    // 2. If the result of a floating point instruction is a NaN, the value written to the result
    // register should be the canonical NaN value. For single-precision, this is 0x7FC00000, for
    // double-precision, this is 0x7FF8000000000000.
    // 3. The 'f' extension uses 32-bit single-precision floating point registers and the
    // 'd' extension uses 64-bit double-precision floating point registers. If both extensions are
    // enabled, then single-precision instructions must write all 1s to the upper 32-bits of
    // the 64-bit register.
    class AtlasRegister
    {
      public:
        using PtrType = sparta::SpartaSharedPointer<AtlasRegister>;

        explicit AtlasRegister(const RegId reg_id) : reg_id_(reg_id) {}

        uint64_t read() { return value_; }

        void write(uint64_t value)
        {
            if (reg_id_.reg_type == RegType::INTEGER and reg_id_.reg_num != 0)
            {
                value_ = value;
            }
        }

        const RegId getRegId() const { return reg_id_; }

        const std::string & getRegName() const { return reg_id_.reg_name; }

      private:
        const RegId reg_id_;
        uint64_t value_ = 0;
    };

    using AtlasRegisterPtr = AtlasRegister::PtrType;

    template <uint64_t Mask> struct RegisterBitMask
    {
        static uint64_t mask(const uint64_t old_val, const uint64_t new_val)
        {
            // The 'Mask' template parameter is a bit mask that specifies which bits are writable.
            // We need to preserve the 'old_val' bits that are not writable, and replace the
            // writable bits with 'new_val'.
            return (old_val & ~Mask) | (new_val & Mask);
        }
    };

    template <> struct RegisterBitMask<0>;

    template <> struct RegisterBitMask<0xffffffffffffffff>
    {
        static uint64_t mask(const uint64_t old_val, const uint64_t new_val)
        {
            (void)old_val;
            return new_val;
        }
    };

    template <typename FieldT, typename Enable = void> struct CSRFields
    {
        static uint64_t readField(const uint64_t reg_val)
        {
            constexpr uint64_t num_field_bits = FieldT::high_bit - FieldT::low_bit + 1;
            constexpr uint64_t mask = (1 << num_field_bits) - 1;
            return (reg_val >> FieldT::low_bit) & mask;
        }
    };

} // namespace atlas
