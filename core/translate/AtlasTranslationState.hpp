#pragma once

#include <cassert>

#include "include/AtlasTypes.hpp"

namespace atlas
{
    class AtlasTranslationState
    {
      public:
        struct TranslationRequest
        {
          public:
            TranslationRequest(Addr vaddr, size_t size) : vaddr_(vaddr), size_(size) {}

            Addr getVAddr() const { return vaddr_; }

            size_t getSize() const { return size_; }

            bool isValid() const { return size_ != 0; }

            void setMisaligned(const size_t misaligned_bytes)
            {
                sparta_assert(misaligned_bytes < size_);
                misaligned_bytes_ = misaligned_bytes;
                misaligned_ = true;
            }

            bool isMisaligned() const { return misaligned_; }

            size_t getMisalignedBytes() const { return misaligned_bytes_; }

          private:
            const Addr vaddr_;
            const size_t size_;
            bool misaligned_ = false;
            size_t misaligned_bytes_ = 0;
        };

        struct TranslationResult
        {
          public:
            TranslationResult(Addr paddr, size_t sz) : paddr_(paddr), size_(sz) {}

            Addr getPAddr() const { return paddr_; }

            size_t getSize() const { return size_; }

            bool isValid() const { return size_ != 0; }

          private:
            const Addr paddr_;
            const size_t size_;
        };

        void makeRequest(const Addr vaddr, const size_t size)
        {
	    sparta_assert(size > 0);
            sparta_assert(results_.empty());
            requests_.emplace(vaddr, size);
        }

        uint32_t getNumRequests() const { return requests_.size(); }

        TranslationRequest & getRequest()
        {
            sparta_assert(requests_.empty() == false);
            return requests_.front();
        }

        void popRequest()
        {
            sparta_assert(requests_.empty() == false);
            requests_.pop();
        }

        void setResult(const Addr paddr, const size_t size) { results_.emplace(paddr, size); }

        uint32_t getNumResults() const { return results_.size(); }

        const TranslationResult & getResult() const
        {
            sparta_assert(results_.empty() == false);
            return results_.front();
        }

        void popResult()
        {
            sparta_assert(results_.empty() == false);
            results_.pop();
        }

        void reset()
        {
            while (requests_.empty() == false)
            {
                requests_.pop();
            }
            while (results_.empty() == false)
            {
                results_.pop();
            }
        }

      private:
        // Translation request
        std::queue<TranslationRequest> requests_;

        // Translation result
        std::queue<TranslationResult> results_;
    };
} // namespace atlas
