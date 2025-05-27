#pragma once

#include "core/ActionGroup.hpp"
#include "mavis/JSONUtils.hpp"
#include "sparta/utils/SpartaSharedPointerAllocator.hpp"

namespace atlas
{
    class AtlasExtractor
    {
      public:
        // Required by Mavis
        using PtrType = sparta::SpartaSharedPointer<AtlasExtractor>;

        AtlasExtractor(const boost::json::object & json, const AtlasState* state);
        AtlasExtractor(const AtlasExtractor &) = default;

        // Required by Mavis
        void update(const boost::json::object &) {}

        // Required by Mavis
        const std::string & getName() const { return mnemonic_; }

        bool isMemoryInst() const { return is_memory_inst_; }

        bool isChangeOfFlowInst() const { return is_cof_inst_; }

      private:
        const std::string mnemonic_;
        const std::string inst_handler_name_;
        const bool is_memory_inst_;
        const bool is_cof_inst_;

        ActionGroup inst_action_group_;

        friend class AtlasInst;
    };

    using AtlasExtractorPtr = AtlasExtractor::PtrType;
    using AtlasExtractorAllocator = sparta::SpartaSharedPointerAllocator<AtlasExtractor>;
} // namespace atlas
