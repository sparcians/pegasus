#pragma once

#include <stdint.h>
#include "../include/AtlasTypes.hpp"

namespace atlas
{
    template <MMUMode Mode> // XLEN
    class PageTableEntry
    {
      public:
        using xLen = typename std::conditional_t<Mode == MMUMode::SV32, RV32, RV64>;

      private:
        xLen pageTableEntryValue_; // 32-bit or 64-bit value representing the page table entry
        uint16_t unUsedBits_;
        xLen PPN_; // PPN will have either uint32_t or uint64_t type
                   // if uint32 --> SV32
                   // 22-bit Physical Page Number
                   // (PPN[1]:22-10 bits)
                   // (PPN[0]:9-0 bits)
        // if uint64 --> SV39/SV48 (based on mode decided)
        // 44-bit Physical Page Number
        // PPN0 (Bits 10–29): 20 bits
        // PPN1 (Bits 30–53): 24 bits
        // PPN2 (Bits 54–63): 10 bits
        uint8_t RSW_; // 2-bit Reserved for Supervisor
        bool D_;      // Dirty bit
        bool A_;      // Accessed bit
        bool G_;      // Global bit
        bool U_;      // User-mode bit
        bool X_;      // Execute bit
        bool W_;      // Write bit
        bool R_;      // Read bit
        bool V_;      // Valid bit

      public:
        PageTableEntry(xLen pte)
        {
            if constexpr (Mode == MMUMode::SV32)
            {
                pageTableEntryValue_ = pte;
                unUsedBits_ = 0;                               // no such field exist here hence 0
                uint32_t PPN1 = (pte >> 20) & TWELVE_BIT_MASK; // 12 bits
                uint32_t PPN0 =
                    (pte >> 10) & TEN_BIT_MASK; // Extract PPN[0] (bits 19-10) // 10 bits
                this->PPN_ = (PPN1 << 10)
                             | PPN0; // Combine PPN1 and PPN0 to form the full PPN // 22 bits total
                decodePTEFields_(pte);
            }
        }

        // Getter methods for each field
        xLen getPTE() const { return pageTableEntryValue_; }

        uint32_t getUnusedBits() const { return (Mode == MMUMode::SV32) ? 0 : unUsedBits_; }

        uint32_t getPPN() const { return PPN_; }

        uint32_t getPPN1() const
        {
            return (Mode == MMUMode::SV32) ? ((PPN_ >> 10) & 0xFFF) : 0;
        } // 12 bits for PPN1

        uint32_t getPPN0() const
        {
            return (Mode == MMUMode::SV32) ? (PPN_ & 0x3FF) : 0;
        } // 10 bits for PPN0

        uint8_t getRSW() const { return RSW_; }

        bool isDirty() const { return D_; }

        bool isAccessed() const { return A_; }

        bool isGlobal() const { return G_; }

        bool isUserMode() const { return U_; }

        bool canExecute() const { return X_; }

        bool canWrite() const { return W_; }

        bool canRead() const { return R_; }

        bool isValid() const { return V_; }

      private:
        // Setter methods for each field
        void setPTE_(xLen pte) { pageTableEntryValue_ = pte; }

        void setUnusedBits_(uint32_t unusedBits)
        {
            if constexpr (Mode != MMUMode::SV32)
            {
                unUsedBits_ = unusedBits;
            }
        }

        void setPPN_(uint32_t ppn) { PPN_ = ppn; } // Setter for full PPN

        void setPPN0_(uint32_t ppn0)
        {
            if constexpr (Mode == MMUMode::SV32)
            {
                PPN_ &= ~0x3FF;         // Clear the lower 10 bits (PPN0)
                PPN_ |= (ppn0 & 0x3FF); // Set the lower 10 bits (PPN0)
            }
        }

        void setPPN1_(uint32_t ppn1)
        {
            if constexpr (Mode == MMUMode::SV32)
            {
                PPN_ &= ~(0xFFF << 10);       // Clear bits 21-10 (PPN1)
                PPN_ |= (ppn1 & 0xFFF) << 10; // Set bits 21-10 (PPN1)
            }
        }

        void setRSW_(uint8_t rsw) { RSW_ = rsw & 0x3; } // Mask to ensure it's a 2-bit value

        void setDirty_(bool d) { D_ = d; }

        void setAccessed_(bool a) { A_ = a; }

        void setGlobal_(bool g) { G_ = g; }

        void setUserMode_(bool u) { U_ = u; }

        void setExecute_(bool x) { X_ = x; }

        void setWrite_(bool w) { W_ = w; }

        void setRead_(bool r) { R_ = r; }

        void setValid_(bool v) { V_ = v; }

        void decodePTEFields_(xLen pte)
        {
            if constexpr (Mode == MMUMode::SV32)
            {
                RSW_ = (pte >> 8) & TWO_BIT_MASK; // Extract RSW_ (bits 9-8) // 2 bits
                D_ = pte & PTE_D_MASK;            // Extract D_ (Dirty bit, bit 7) // 1 bit
                A_ = pte & PTE_A_MASK;            // Extract A_ (Accessed bit, bit 6) // 1 bit
                G_ = pte & PTE_G_MASK;            // Extract G_ (Global bit, bit 5) // 1 bit
                U_ = pte & PTE_U_MASK;            // Extract U_ (User-mode bit, bit 4) // 1 bit
                X_ = pte & PTE_X_MASK;            // Extract X_ (Execute bit, bit 3) // 1 bit
                W_ = pte & PTE_W_MASK;            // Extract W_ (Write bit, bit 2) // 1 bit
                R_ = pte & PTE_R_MASK;            // Extract R_ (Read bit, bit 1) // 1 bit
                V_ = pte & PTE_V_MASK;            // Extract V_ (Valid bit, bit 0) // 1 bit
            }
        }
    };
} // namespace atlas
