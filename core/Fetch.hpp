#pragma once

#include "core/ActionGroup.hpp"
#include "core/AtlasAllocatorWrapper.hpp"
#include "sim/AtlasAllocators.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/TreeNode.hpp"
#include "sparta/simulation/Unit.hpp"

template <class InstT, class ExtenT, class InstTypeAllocator, class ExtTypeAllocator> class Mavis;

namespace atlas
{
    class AtlasState;

    using MavisType =
        Mavis<AtlasInst, AtlasExtractor, AtlasInstAllocatorWrapper<AtlasInstAllocator>,
              AtlasExtractorAllocatorWrapper<AtlasExtractorAllocator>>;

    class Fetch : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "Fetch";
        using base_type = Fetch;

        class FetchParameters : public sparta::ParameterSet
        {
          public:
            FetchParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            PARAMETER(std::string, isa_file_path, "mavis_json", "Where are the Mavis isa files?")
            PARAMETER(std::string, uarch_file_path, "arch", "Where are the Atlas uarch files?")
        };

        Fetch(sparta::TreeNode* fetch_node, const FetchParameters* p);

        ActionGroup* getActionGroup() { return &fetch_action_group_; }

      private:
        const std::string isa_file_path_;
        const std::string uarch_file_path_;

        AtlasState* state_ = nullptr;

        void onBindTreeEarly_() override;

        ActionGroup* fetch_(atlas::AtlasState* state);

        ActionGroup fetch_action_group_{"Fetch"};

        ActionGroup* decode_(atlas::AtlasState* state);

        ActionGroup decode_action_group_{"Decode"};

        void advanceSim_();

        // Mavis
        std::unique_ptr<MavisType> mavis_;
    };
} // namespace atlas
