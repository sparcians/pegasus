namespace atlas
{
    // Constructor to initialize from a 32-bit PTE value
    template <>
    PageTableEntry<uint32_t, MMUMode::SV32>::PageTableEntry(uint32_t pte){
        pageTableEntryValue_ = pte;
        unUsedBits_ = 0;   //no such field exist here hence 0
        uint32_t PPN1 = (pte >> 20) & TWELVE_BIT_MASK;   // 12 bits
        uint32_t PPN0 = (pte >> 10) & TEN_BIT_MASK;   // Extract PPN[0] (bits 19-10) // 10 bits
        this->PPN_ = (PPN1 << 10) | PPN0;       // Combine PPN1 and PPN0 to form the full PPN // 22 bits total
        decodePTEFields_(pte);
    }

    template <>
    void PageTableEntry<uint32_t, MMUMode::SV32>::setUnusedBits_(uint32_t unusedBits){
        (void)unusedBits;
        unUsedBits_ = 0;    //unUsedBits not present in PTE
    }

    template <>
    void PageTableEntry<uint32_t, MMUMode::SV32>::setPPN_(uint32_t ppn) {PPN_ = ppn;} // Setter for full PPN

    template <> // Setter for lower 10 bits (PPN0)
    void PageTableEntry<uint32_t, MMUMode::SV32>::setPPN0_(uint32_t ppn0) {
        PPN_ &= ~0x3FF;           // Clear the lower 10 bits (PPN0)
        PPN_ |= (ppn0 & 0x3FF);   // Set the lower 10 bits (PPN0) 
    }

    template <> // Setter for upper 12 bits (PPN1)
    void PageTableEntry<uint32_t, MMUMode::SV32>::setPPN1_(uint32_t ppn1) {
        PPN_ &= ~(0xFFF << 10);        // Clear bits 21-10 (PPN1)
        PPN_ |= (ppn1 & 0xFFF) << 10;  // Set bits 21-10 (PPN1) 
    }

    template <>
    uint32_t PageTableEntry<uint32_t, MMUMode::SV32>::getPPN() const {return PPN_;}

    template <>
    uint32_t PageTableEntry<uint32_t, MMUMode::SV32>::getPPN1() const {return (PPN_ >> 10) & 0xFFF;}  // 12 bits for PPN1

    template <>
    uint32_t PageTableEntry<uint32_t, MMUMode::SV32>::getPPN0() const {return PPN_ & 0x3FF;}  // 10 bits for PPN0

    template <>
    uint32_t PageTableEntry<uint32_t, MMUMode::SV32>::getUnusedBits() const {return 0;}

    template <typename xLen, MMUMode Mode>
    void PageTableEntry<xLen, Mode>::decodePTEFields_(xLen pte) {
        RSW_ = (pte >> 8) & TWO_BIT_MASK;           // Extract RSW_ (bits 9-8) // 2 bits
        D_ = pte & PTE_D_MASK;                      // Extract D_ (Dirty bit, bit 7) // 1 bit
        A_ = pte & PTE_A_MASK;                      // Extract A_ (Accessed bit, bit 6) // 1 bit
        G_ = pte & PTE_G_MASK;                      // Extract G_ (Global bit, bit 5) // 1 bit
        U_ = pte & PTE_U_MASK;                      // Extract U_ (User-mode bit, bit 4) // 1 bit
        X_ = pte & PTE_X_MASK;                      // Extract X_ (Execute bit, bit 3) // 1 bit
        W_ = pte & PTE_W_MASK;                      // Extract W_ (Write bit, bit 2) // 1 bit
        R_ = pte & PTE_R_MASK;                      // Extract R_ (Read bit, bit 1) // 1 bit
        V_ = pte & PTE_V_MASK;                      // Extract V_ (Valid bit, bit 0) // 1 bit
    }
}