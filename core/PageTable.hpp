#pragma once

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

        bool isValidIndex(uint32_t idx)
        {
            uint32_t pte_size = (Mode == MMUMode::SV32) ? sizeof(RV32) : sizeof(RV64);
            return ((idx >= baseAddressOfpageTable) &&
                    (idx < ((max_entries * pte_size) + baseAddressOfpageTable)));
        }

        size_t size() const
        {
            return pageTable.size();
        }

        uint16_t getMaxNumEntries() const
        {
            if (Mode == MMUMode::SV32)
                return PT_ENTRIES_SV32;
            else if (Mode == MMUMode::SV39)
                return PT_ENTRIES_SV39;
            return 0;
        }

      public:
        std::map<uint32_t, PageTableEntry<Mode>> pageTable; // uint32_t accommodates all types of VPNs for addressing the PTE

        PageTable(uint32_t baseAddr)
            : max_entries(getMaxNumEntries()), baseAddressOfpageTable(baseAddr) {}

        void addEntry(uint32_t index, PageTableEntry<Mode> entry)
        {
            if (size() <= max_entries && isValidIndex(index)) {
                auto it = pageTable.find(index);
                if (it != pageTable.end()) {
                    it->second = entry; // Update the existing entry
                } else {
                    pageTable.insert({index, entry}); // Insert the new entry
                }
            } else {
                throw std::runtime_error("Page table has reached its maximum capacity or index is out of bounds!");
            }
        }

        PageTableEntry<Mode> getEntry(uint32_t index)
        {
            if (isValidIndex(index)) {
                auto it = pageTable.find(index);
                if (it != pageTable.end()) {
                    return it->second;
                } else {
                    throw std::out_of_range("Index not found in the page table");
                }
            } else {
                throw std::out_of_range("Invalid index");
            }
        }

        void removeEntry(uint32_t index)
        {
            if (isValidIndex(index)) {
                auto it = pageTable.find(index);
                if (it != pageTable.end()) {
                    pageTable.erase(it);
                } else {
                    throw std::out_of_range("Index not found in the page table");
                }
            } else {
                throw std::out_of_range("Invalid index");
            }
        }

        bool contains(uint32_t index) const
        {
            return pageTable.find(index) != pageTable.end();
        }

        void print(std::ostream &os) const
        {
            for (const auto &[index, entry] : pageTable) {
                os << "Index: " << index << " | PTE: " << entry.getPTE() << std::endl;
            }
        }
    };

    template <MMUMode Mode>
    std::ostream &operator<<(std::ostream &os, const PageTable<Mode> &pt)
    {
        pt.print(os);
        return os;
    }
} // namespace atlas
