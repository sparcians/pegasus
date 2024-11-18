#include <map>
#include <iostream>
#include "../include/AtlasTypes.hpp"

namespace atlas
{
    template <MMUMode Mode> // XLEN
    class PageTable
    {
      private:
        const uint16_t max_entries;
        const uint32_t baseAddressOfpageTable;
        bool isValidIndex(uint32_t idx);
        size_t size();
        uint16_t getMaxNumEntries(MMUMode mode) const;

      public:
        std::map<uint32_t, PageTableEntry<Mode>>
            pageTable; // uint32_t will be able to accomodate all the types of indexes(VPNs) for
                       // addressing the PTE

        PageTable(uint32_t baseAddr);
        void addEntry(uint32_t index, PageTableEntry<Mode> entry);
        PageTableEntry<Mode> getEntry(uint32_t index);
        void removeEntry(uint32_t index);
        void print(std::ostream & os);
        bool contains(uint32_t index);
    };

    template <MMUMode Mode>
    std::ostream & operator<<(std::ostream & os, const PageTable<Mode> & pt);
} // namespace atlas

#include "PageTable.tpp"