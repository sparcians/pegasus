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
        PageTableEntry(xLen pte);

        // Getter methods for each field
        xLen getPTE() const;
        uint32_t getUnusedBits() const;
        uint32_t getPPN() const;
        uint32_t getPPN1() const;
        uint32_t getPPN0() const;
        uint8_t getRSW() const;
        bool isDirty() const;
        bool isAccessed() const;
        bool isGlobal() const;
        bool isUserMode() const;
        bool canExecute() const;
        bool canWrite() const;
        bool canRead() const;
        bool isValid() const;

      private:
        // Setter methods for each field
        void setPTE_(xLen pte);
        void setUnusedBits_(uint32_t unusedBits);
        void setPPN_(uint32_t ppn);
        void setPPN0_(uint32_t ppn0);
        void setPPN1_(uint32_t ppn1);
        void setRSW_(uint8_t rsw);
        void setDirty_(bool d);
        void setAccessed_(bool a);
        void setGlobal_(bool g);
        void setUserMode_(bool u);
        void setExecute_(bool x);
        void setWrite_(bool w);
        void setRead_(bool r);
        void setValid_(bool v);

        void decodePTEFields_(xLen pte);
    };

    template <MMUMode Mode> void PageTableEntry<Mode>::setPTE_(xLen pte)
    {
        pageTableEntryValue_ = pte;
    }

    template <MMUMode Mode> void PageTableEntry<Mode>::setRSW_(uint8_t rsw)
    {
        RSW_ = rsw & 0x3;
    } // Mask to ensure it's a 2-bit value

    template <MMUMode Mode> void PageTableEntry<Mode>::setDirty_(bool d) { D_ = d; }

    template <MMUMode Mode> void PageTableEntry<Mode>::setAccessed_(bool a) { A_ = a; }

    template <MMUMode Mode> void PageTableEntry<Mode>::setGlobal_(bool g) { G_ = g; }

    template <MMUMode Mode> void PageTableEntry<Mode>::setUserMode_(bool u) { U_ = u; }

    template <MMUMode Mode> void PageTableEntry<Mode>::setExecute_(bool x) { X_ = x; }

    template <MMUMode Mode> void PageTableEntry<Mode>::setWrite_(bool w) { W_ = w; }

    template <MMUMode Mode> void PageTableEntry<Mode>::setRead_(bool r) { R_ = r; }

    template <MMUMode Mode> void PageTableEntry<Mode>::setValid_(bool v) { V_ = v; }

    template <MMUMode Mode> typename PageTableEntry<Mode>::xLen PageTableEntry<Mode>::getPTE() const
    {
        return pageTableEntryValue_;
    }

    template <MMUMode Mode> uint8_t PageTableEntry<Mode>::getRSW() const { return RSW_; }

    template <MMUMode Mode> bool PageTableEntry<Mode>::isDirty() const { return D_; }

    template <MMUMode Mode> bool PageTableEntry<Mode>::isAccessed() const { return A_; }

    template <MMUMode Mode> bool PageTableEntry<Mode>::isGlobal() const { return G_; }

    template <MMUMode Mode> bool PageTableEntry<Mode>::isUserMode() const { return U_; }

    template <MMUMode Mode> bool PageTableEntry<Mode>::canExecute() const { return X_; }

    template <MMUMode Mode> bool PageTableEntry<Mode>::canWrite() const { return W_; }

    template <MMUMode Mode> bool PageTableEntry<Mode>::canRead() const { return R_; }

    template <MMUMode Mode> bool PageTableEntry<Mode>::isValid() const { return V_; }
} // namespace atlas

#include "PageTableEntry.tpp"