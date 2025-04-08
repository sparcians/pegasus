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
          private:
            Addr virtual_addr_ = 0x0;
            size_t size_ = 0;

          public:
            TranslationRequest() = default;

            TranslationRequest(Addr vaddr, size_t sz) : virtual_addr_(vaddr), size_(sz) {}

            Addr getVaddr() const { return virtual_addr_; }

            size_t getSize() const { return size_; }

            bool isValid() const { return size_ != 0; }
        };

        struct TranslationResult
        {
          private:
            Addr physical_addr_ = 0x0;
            size_t size_ = 0;

          public:
            TranslationResult() = default;

            TranslationResult(Addr paddr, size_t sz) : physical_addr_(paddr), size_(sz) {}

            Addr getPaddr() const { return physical_addr_; }

            size_t getSize() const { return size_; }

            bool isValid() const { return size_ != 0; }
        };

        void makeRequest(const Addr virtual_addr, const size_t size)
        {
            sparta_assert(request_.isValid() == false);

            request_ = TranslationRequest(virtual_addr, size);

            // Invalidate previous result
            result_ = TranslationResult();
        }

        const TranslationRequest & getRequest() const
        {
            sparta_assert(request_.isValid());

            return request_;
        }

        void clearRequest() { request_ = TranslationRequest(); }

        void setResult(const Addr physical_addr, const size_t size)
        {
            sparta_assert(request_.isValid());

            result_ = TranslationResult(physical_addr, size);

            // Invalidate request
            request_ = TranslationRequest();
        }

        const TranslationResult & getResult() const
        {
            sparta_assert(result_.isValid());

            return result_;
        }

      private:
        // Translation request
        TranslationRequest request_;

        // Translation result
        TranslationResult result_;
    };
} // namespace atlas
