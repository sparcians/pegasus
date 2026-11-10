#pragma once

#include "core/ExecutionPage.hpp"

namespace pegasus
{
    class ExecutionCache
    {
      public:
        ExecutionCache(const bool ecache_stats) : ecache_stats_(ecache_stats) {}

        // Request a translated execution-page cache flush at the next safe fetch boundary.
        void requestExecutionCacheFlush() { execution_cache_flush_requested_ = true; }

        // Flushes the execution cache by clearing the VA-index owning table.
        // Clears translated execution-page cache and VA-index structure.
        void flushExecutionCache() { execution_page_table_.clear(); }

        ActionGroup* lookupExecutionPageGroup(const Addr vaddr) const;

        ActionGroup* registerExecutionPageResult(PegasusState* state, const Addr vaddr,
                                                 const Addr paddr, const Addr page_size);

        // Returns true exactly once per request and clears the pending flag.
        bool consumeExecutionCacheFlushRequest()
        {
            const bool do_flush = execution_cache_flush_requested_;
            execution_cache_flush_requested_ = false;
            return do_flush;
        }

        // Execute cache stats
        void recordExecCacheDecision(bool instruction_reuse)
        {
            if (instruction_reuse)
            {
                ++exec_cache_reuse_count_;
            }
            else
            {
                ++exec_cache_first_decode_count_;
            }
        }

        // Count only translation operations that perform an MMU page walk
        // (i.e., not machine-mode/baremetal short-circuit translation).
        void recordTranslationRequest() { ++translation_request_count_; }

        // Count translation operations that bypass page walks and return immediately
        // due to machine mode or baremetal translation mode.
        void recordTranslationBypass() { ++translation_bypass_count_; }

        void recordPageWalkTranslation() { ++page_walk_translation_count_; }

        void recordExecCacheBypassEnter() { ++exec_cache_bypass_enter_count_; }

        void recordExecCacheBypassFallback() { ++exec_cache_bypass_fallback_count_; }

        // Count how often setupInst wires NCR directly to the next instruction
        // action group, bypassing translatedPageExecute_ for subsequent hits.
        void recordExecCachePteBypassSetup() { ++exec_cache_pte_bypass_setup_count_; }

        uint64_t getExecCacheReuseCount() const { return exec_cache_reuse_count_; }

        uint64_t getExecCacheFirstDecodeCount() const { return exec_cache_first_decode_count_; }

        uint64_t getExecCacheBypassEnterCount() const { return exec_cache_bypass_enter_count_; }

        uint64_t getExecCacheBypassFallbackCount() const
        {
            return exec_cache_bypass_fallback_count_;
        }

        uint64_t getExecCachePteBypassSetupCount() const
        {
            return exec_cache_pte_bypass_setup_count_;
        }

        uint64_t getTranslationRequestCount() const { return translation_request_count_; }

        uint64_t getTranslationBypassCount() const { return translation_bypass_count_; }

        uint64_t getPageWalkTranslationCount() const { return page_walk_translation_count_; }

        double getExecCacheReuseRatio() const
        {
            const uint64_t total = exec_cache_reuse_count_ + exec_cache_first_decode_count_;
            return (total == 0) ? 0.0 : static_cast<double>(exec_cache_reuse_count_) / total;
        }

        void enable() { ecache_enabled_ = true; }

        bool isEnabled() const { return ecache_enabled_; }

        bool areStatsEnabled() const { return ecache_stats_; }

      private:
        //! Whether the execution cache (ecache) fast path is enabled
        bool ecache_enabled_ = false;

        //! Print execution-cache counter summary when simulation stops
        const bool ecache_stats_;

        // Two-level VA lookup table indexed with (VA >> 12), matching Sv39 page table bitmasking
        // The table owns pages via shared_ptr so superpage spans can reference
        // the same ExecutionPage from multiple VA L0 entries.
        using ExecutionPageL0Table = std::unordered_map<Addr, std::shared_ptr<ExecutionPage>>;
        using ExecutionPageL1Table = std::unordered_map<Addr, ExecutionPageL0Table>;
        std::unordered_map<Addr, ExecutionPageL1Table> execution_page_table_;

        // Execution cache decision counters
        uint64_t exec_cache_reuse_count_ = 0;
        uint64_t exec_cache_first_decode_count_ = 0;
        uint64_t exec_cache_bypass_enter_count_ = 0;
        uint64_t exec_cache_bypass_fallback_count_ = 0;
        uint64_t exec_cache_pte_bypass_setup_count_ = 0;
        uint64_t translation_request_count_ = 0;
        uint64_t translation_bypass_count_ = 0;
        uint64_t page_walk_translation_count_ = 0;

        // Deferred cache flush flag to avoid invalidating active action groups mid-execution.
        bool execution_cache_flush_requested_ = false;

        friend std::ostream & operator<<(std::ostream & os, const ExecutionCache & ecache);
        friend std::ostream & operator<<(std::ostream & os, const ExecutionCache* ecache);
    };

    inline std::ostream & operator<<(std::ostream & os, const ExecutionCache & ecache)
    {
        const uint64_t reuse_count = ecache.getExecCacheReuseCount();
        const uint64_t first_decode_count = ecache.getExecCacheFirstDecodeCount();
        const uint64_t total_decisions = reuse_count + first_decode_count;
        os << "Execution Cache statistics" << std::endl;
        os << "\tExecCache decisions: " << total_decisions << " (reuse=" << reuse_count
           << ", first_decode = " << first_decode_count << ")" << std::endl;
        os << "\tExecCache reuse ratio: " << ecache.getExecCacheReuseRatio() << std::endl;
        os << "\tExecCache bypass: enter = " << ecache.getExecCacheBypassEnterCount()
           << ", fallback = " << ecache.getExecCacheBypassFallbackCount()
           << ", pTE_setup = " << ecache.getExecCachePteBypassSetupCount() << std::endl;
        os << "\tTranslation requests: " << ecache.getTranslationRequestCount() << std::endl;
        os << "\tTranslation bypass (machine/baremetal): " << ecache.getTranslationBypassCount()
           << std::endl;
        os << "\tMMU page-walk translations: " << ecache.getPageWalkTranslationCount() << std::endl;

        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const ExecutionCache* ecache)
    {
        os << *ecache;
        return os;
    }
} // namespace pegasus
