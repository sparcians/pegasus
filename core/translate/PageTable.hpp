#pragma once

#include <map>
#include <iostream>
#include "../include/AtlasTypes.hpp"

namespace atlas
{

    uint16_t determineMaxNumPteEntries(MMUMode mode)
    {
        if (mode == MMUMode::SV32)
        {
            return 1024;
        }
        else if (mode == MMUMode::SV39)
        {
            return 512;
        }
        else
        {
            sparta_assert(false, "Unsupported MMU Mode!");
        }
    }

    template <typename XLEN, MMUMode Mode> class PageTable
    {
      public:
        PageTable(uint32_t base_addr) :
            max_entries_(determineMaxNumPteEntries(Mode)),
            base_addr_(base_addr)
        {
        }

        void addEntry(uint32_t idx, PageTableEntry<XLEN, Mode> entry)
        {
            if (page_table_.size() <= max_entries_ && isValidIndex_(idx))
            {
                auto it = page_table_.find(idx);
                if (it != page_table_.end())
                {
                    it->second = entry; // Update the existing entry
                }
                else
                {
                    page_table_.insert({idx, entry}); // Insert the new entry
                }
            }
            else
            {
                throw std::runtime_error(
                    "Page table has reached its maximum capacity or idx is out of bounds!");
            }
        }

        PageTableEntry<XLEN, Mode> getEntry(uint32_t idx)
        {
            if (isValidIndex_(idx))
            {
                auto it = page_table_.find(idx);
                if (it != page_table_.end())
                {
                    return it->second;
                }
                else
                {
                    throw std::out_of_range("Index not found in the page table");
                }
            }
            else
            {
                throw std::out_of_range("Invalid idx");
            }
        }

        void removeEntry(uint32_t idx)
        {
            if (isValidIndex_(idx))
            {
                auto it = page_table_.find(idx);
                if (it != page_table_.end())
                {
                    page_table_.erase(it);
                }
                else
                {
                    throw std::out_of_range("Index not found in the page table");
                }
            }
            else
            {
                throw std::out_of_range("Invalid idx");
            }
        }

        bool contains(uint32_t idx) const { return page_table_.find(idx) != page_table_.end(); }

      private:
        std::map<uint32_t, PageTableEntry<XLEN, Mode>> page_table_;
        const uint16_t max_entries_;
        const uint32_t base_addr_;

        bool isValidIndex_(uint32_t idx)
        {
            const uint32_t pte_size = (Mode == MMUMode::SV32) ? sizeof(RV32) : sizeof(RV64);
            return ((idx >= base_addr_) && (idx < ((max_entries_ * pte_size) + base_addr_)));
        }

        friend std::ostream & operator<<(std::ostream & os, const PageTable<XLEN, Mode> & pt);
    };

    template <typename XLEN, MMUMode Mode>
    std::ostream & operator<<(std::ostream & os, const PageTable<XLEN, Mode> & pt)
    {
        for (const auto & [idx, entry] : pt.page_table_)
        {
            os << "Index: " << idx << " | PTE: 0x" << std::hex << entry.getPte() << std::endl;
        }
        return os;
    }
} // namespace atlas
