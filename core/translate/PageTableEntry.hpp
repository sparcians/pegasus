#pragma once

#include <stdint.h>
#include "include/AtlasTypes.hpp"
#include "core/translate/TranslateTypes.hpp"

#include "sparta/utils/LogUtils.hpp"

namespace atlas
{
    template <typename XLEN, MMUMode Mode> class PageTableEntry
    {
      public:
        PageTableEntry(XLEN pte) : pte_val_(pte) { decodePteFields_(); }

        XLEN getPte() const { return pte_val_; }

        XLEN getPpn() const { return ppn_; }

        XLEN getPpnField(const uint32_t idx) const { return ppn_fields_.at(idx); }

        uint32_t getRsw() const { return rsw_; }

        bool isDirty() const { return d_; }

        bool isAccessed() const { return a_; }

        bool isGlobal() const { return g_; }

        bool isUserMode() const { return u_; }

        bool canExecute() const { return x_; }

        bool canWrite() const { return w_; }

        bool canRead() const { return r_; }

        bool isValid() const { return v_; }

        bool isLeaf() const { return canRead() || canExecute(); }

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
                ppn_fields_.resize(translate_types::Sv32::num_ppn_fields);
                ppn_fields_[0] = (pte_val_ & translate_types::Sv32::PteFields::ppn0.bitmask)
                                 >> translate_types::Sv32::PteFields::ppn0.lsb;
                ppn_fields_[1] = (pte_val_ & translate_types::Sv32::PteFields::ppn1.bitmask)
                                 >> translate_types::Sv32::PteFields::ppn1.lsb;

                // Combine PPN1 and PPN0 to form the full PPN, 22 bits total
                ppn_ = (pte_val_
                        & (translate_types::Sv32::PteFields::ppn1.bitmask
                           | translate_types::Sv32::PteFields::ppn0.bitmask))
                       >> translate_types::Sv32::PteFields::ppn0.lsb;
            }
            else if constexpr (Mode == MMUMode::SV39)
            {
                ppn_fields_.resize(translate_types::Sv39::num_ppn_fields);
                ppn_fields_[0] = (pte_val_ & translate_types::Sv39::PteFields::ppn0.bitmask)
                                 >> translate_types::Sv39::PteFields::ppn0.lsb;
                ppn_fields_[1] = (pte_val_ & translate_types::Sv39::PteFields::ppn1.bitmask)
                                 >> translate_types::Sv39::PteFields::ppn1.lsb;
                ppn_fields_[2] = (pte_val_ & translate_types::Sv39::PteFields::ppn2.bitmask)
                                 >> translate_types::Sv39::PteFields::ppn2.lsb;

                // Combine PPN1 and PPN0 to form the full PPN, 22 bits total
                ppn_ = (pte_val_
                        & (translate_types::Sv39::PteFields::ppn2.bitmask
                           | translate_types::Sv39::PteFields::ppn1.bitmask
                           | translate_types::Sv39::PteFields::ppn0.bitmask))
                       >> translate_types::Sv39::PteFields::ppn0.lsb;
            }
            else
            {
                sparta_assert(false, "MMU mode is not currently supported!");
            }

            // Lower 10 bits are the same for all modes
            rsw_ = (pte_val_ & translate_types::Sv32::PteFields::rsw.bitmask)
                   >> translate_types::Sv32::PteFields::rsw.lsb;
            d_ = pte_val_ & translate_types::Sv32::PteFields::dirty.bitmask;
            a_ = pte_val_ & translate_types::Sv32::PteFields::accessed.bitmask;
            g_ = pte_val_ & translate_types::Sv32::PteFields::global.bitmask;
            u_ = pte_val_ & translate_types::Sv32::PteFields::user.bitmask;
            x_ = pte_val_ & translate_types::Sv32::PteFields::execute.bitmask;
            w_ = pte_val_ & translate_types::Sv32::PteFields::write.bitmask;
            r_ = pte_val_ & translate_types::Sv32::PteFields::read.bitmask;
            v_ = pte_val_ & translate_types::Sv32::PteFields::valid.bitmask;
        }
    };

    template <typename XLEN, MMUMode Mode>
    std::ostream & operator<<(std::ostream & os, const PageTableEntry<XLEN, Mode> & pte)
    {
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            os << HEX16(pte.getPte());
        }
        else
        {
            os << HEX8(pte.getPte());
        }
        return os;
    }
} // namespace atlas
