#include "core/ExecutionCache.hpp"
#include "core/PegasusState.hpp"
#include "core/Fetch.hpp"
#include "core/Execute.hpp"

namespace pegasus
{
    namespace
    {
        constexpr Addr kExecPageShift = 12;
        // Masks kept separate to allow for easy modification
        constexpr Addr kExecPageL0Bits = 9; // [20:12]
        constexpr Addr kExecPageL1Bits = 9; // [29:21]
        constexpr Addr kExecPageL2Bits = 9; // [38:30]
        constexpr Addr kExecPageL0Mask = (Addr(1) << kExecPageL0Bits) - 1;
        constexpr Addr kExecPageL1Mask = (Addr(1) << kExecPageL1Bits) - 1;
        constexpr Addr kExecPageL2Mask = (Addr(1) << kExecPageL2Bits) - 1;

        inline Addr getExecPageL0Index(const Addr vaddr)
        {
            return (vaddr >> kExecPageShift) & kExecPageL0Mask;
        }

        inline Addr getExecPageL1Index(const Addr vaddr)
        {
            return ((vaddr >> kExecPageShift) >> kExecPageL1Bits) & kExecPageL1Mask;
        }

        inline Addr getExecPageL2Index(const Addr vaddr)
        {
            return ((vaddr >> kExecPageShift) >> (kExecPageL1Bits + kExecPageL2Bits))
                   & kExecPageL2Mask;
        }
    } // namespace

    ActionGroup* ExecutionCache::lookupExecutionPageGroup(const Addr vaddr) const
    {
        const Addr l2_idx = getExecPageL2Index(vaddr);
        const auto l2_it = execution_page_table_.find(l2_idx);
        // If L2 index is not found, there are no cached pages for any VA in this L2 span.
        if (l2_it == execution_page_table_.end())
        {
            return nullptr;
        }

        const Addr l1_idx = getExecPageL1Index(vaddr);
        const auto l1_it = l2_it->second.find(l1_idx);
        // If L1 index is not found, there are no cached pages for any VA in this L1 span.
        if (l1_it == l2_it->second.end())
        {
            return nullptr;
        }

        const Addr l0_idx = getExecPageL0Index(vaddr);
        const auto l0_it = l1_it->second.find(l0_idx);
        // If L0 index is not found or the execution page is null, there are no cached pages for
        // this VA.
        if (l0_it == l1_it->second.end() || !l0_it->second)
        {
            return nullptr;
        }

        return l0_it->second->getExecutionPageActionGroup();
    }

    // Registers a newly translated execution page for the given VA, PAs, and page size.
    // The VA table is the owning structure and stores shared page references.
    ActionGroup* ExecutionCache::registerExecutionPageResult(PegasusState* state, const Addr vaddr,
                                                             const Addr paddr, const Addr page_size)
    {
        if ((page_size < 0x1000) || ((page_size & (page_size - 1)) != 0))
        {
            return nullptr;
        }

        const Addr page_mask = page_size - 1;
        const Addr vaddr_base = vaddr & ~page_mask;
        const Addr l2_idx = getExecPageL2Index(vaddr_base);
        const Addr l1_idx = getExecPageL1Index(vaddr_base);
        const auto vpn = vaddr >> kExecPageShift;
        const Addr vpn_l2_idx = (vpn >> (kExecPageL1Bits + kExecPageL0Bits)) & kExecPageL2Mask;
        const Addr vpn_l1_idx = (vpn >> kExecPageL0Bits) & kExecPageL1Mask;
        const Addr vpn_l0_idx = vpn & kExecPageL0Mask;
        auto & l0_table = execution_page_table_[l2_idx][l1_idx];

        // Only skip insertion if this exact 4K VPN slot already has a page.
        if (auto existing_it = l0_table.find(vpn_l0_idx);
            existing_it != l0_table.end() && existing_it->second)
        {
            return existing_it->second->getExecutionPageActionGroup();
        }

        // const Addr paddr_base = paddr & ~page_mask;
        // ILOG("Creating new execution page for vaddr 0x" << std::hex << vaddr_base << " paddr 0x"
        //                                                 << paddr_base << " size 0x" <<
        //                                                 page_size);

        PegasusTranslationState::TranslationResult cached_result(vaddr, paddr, sizeof(Opcode),
                                                                 page_size);
        auto exec_page =
            std::make_shared<ExecutionPage>(cached_result, state->getFetchUnit()->getActionGroup(),
                                            state->getExecuteUnit()->getActionGroup());

        // Insert only the single VPN entry for this access (lazy population).
        // For superpages, other 4K chunks within the same page will miss on first access,
        // fall through to translation (which returns the same superpage result),
        // and lazily add their own entry pointing to the same shared ExecutionPage.
        // This avoids O(page_size/4K) insertions for 2MB/1GB superpages.
        execution_page_table_[vpn_l2_idx][vpn_l1_idx][vpn_l0_idx] = exec_page;
        return exec_page->getExecutionPageActionGroup();
    }
} // namespace pegasus
