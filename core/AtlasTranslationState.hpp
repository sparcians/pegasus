#pragma once

#include "include/AtlasTypes.hpp"

namespace atlas
{
    class AtlasTranslationState
    {
      public:
        struct TranslationRequest
        {
            Addr virtual_addr = 0x0;
            size_t size = 0;

            Addr getVaddr() const { return virtual_addr; }

            bool isValid() const { return size != 0; }
        };

        struct TranslationResult
        {
            Addr physical_addr = 0x0;
            size_t size = 0;

            Addr getPaddr() const { return physical_addr; }

            bool isValid() const { return size != 0; }
        };

        void makeTranslationRequest(const Addr virtual_addr, const size_t size)
        {
            sparta_assert(request_.isValid() == false,
                          "Cannot make another translation request because the current "
                          "request has not beeen unresolved!");

            request_.virtual_addr = virtual_addr;
            request_.size = size;

            // Invalidate previous result
            result_.size = 0;
        }

        const TranslationRequest & getTranslationRequest() const
        {
            sparta_assert(request_.isValid(),
                          "Cannot get the current translation request because it is not valid!");

            return request_;
        }

        void setTranslationResult(const Addr physical_addr, const size_t size)
        {
            sparta_assert(request_.isValid(),
                          "Cannot set the translation result because the current "
                          "translation request is not valid!");

            result_.physical_addr = physical_addr;
            result_.size = size;

            // Invalidate request
            request_.size = 0;
        }

        const TranslationResult & getTranslationResult() const
        {
            sparta_assert(result_.isValid(),
                          "Cannot get the current translation result because it is not valid!");

            return result_;
        }

      private:
        // Translation request
        TranslationRequest request_;

        // Translation result
        TranslationResult result_;
    };
} // namespace atlas
