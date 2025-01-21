#pragma once

#include <stdint.h>
#include "include/AtlasTypes.hpp"
#include "include/PteDefns.hpp"

namespace atlas
{
    template <typename XLEN, MMUMode Mode> class PageTableEntry
    {
      public:
        PageTableEntry(XLEN pte) : pte_val_(pte) { decodePteFields_(); }

        XLEN getPte() const { return pte_val_; }

        uint32_t getPpn() const { return ppn_; }

        uint32_t getPpnField(const uint32_t idx) const { return ppn_fields_.at(idx); }

        uint32_t getRsw() const { return rsw_; }

        bool isDirty() const { return d_; }

        bool isAccessed() const { return a_; }

        bool isGlobal() const { return g_; }

        bool isUserMode() const { return u_; }

        bool canExecute() const { return x_; }

        bool canWrite() const { return w_; }

        bool canRead() const { return r_; }

        bool isValid() const { return v_; }

      private:
        XLEN pte_val_;
        XLEN ppn_;
        std::vector<XLEN> ppn_fields_;

        uint32_t rsw_; // 2-bit Reserved for Supervisor
        bool d_;       // Dirty bit
        bool a_;       // Accessed bit
        bool g_;       // Global bit
        bool u_;       // User-mode bit
        bool x_;       // Execute bit
        bool w_;       // Write bit
        bool r_;       // Read bit
        bool v_;       // Valid bit

        void decodePteFields_()
        {
            if constexpr (Mode == MMUMode::SV32)
            {
                ppn_fields_.resize(Sv32Pte::num_ppn_fields);
                ppn_fields_[0] = (pte_val_ & Sv32Pte::ppn0::bitmask) >> Sv32Pte::ppn0::low_bit;
                ppn_fields_[1] = (pte_val_ & Sv32Pte::ppn1::bitmask) >> Sv32Pte::ppn1::low_bit;

                // Combine PPN1 and PPN0 to form the full PPN, 22 bits total
                ppn_ = (ppn_fields_[1] << 10) | ppn_fields_[0];

                rsw_ = (pte_val_ & Sv32Pte::rsw::bitmask) >> Sv32Pte::rsw::low_bit;
                d_ = pte_val_ & Sv32Pte::dirty::bitmask;
                a_ = pte_val_ & Sv32Pte::accessed::bitmask;
                g_ = pte_val_ & Sv32Pte::global::bitmask;
                u_ = pte_val_ & Sv32Pte::user::bitmask;
                x_ = pte_val_ & Sv32Pte::execute::bitmask;
                w_ = pte_val_ & Sv32Pte::write::bitmask;
                r_ = pte_val_ & Sv32Pte::read::bitmask;
                v_ = pte_val_ & Sv32Pte::valid::bitmask;
            }
        }
    };
} // namespace atlas
