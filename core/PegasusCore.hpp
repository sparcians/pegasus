#pragma once

#include <vector>
#include <string>
#include <cinttypes>

#include "core/PegasusState.hpp"
#include "core/Fetch.hpp"
#include "core/translate/Translate.hpp"
#include "core/Execute.hpp"
#include "core/Exception.hpp"

#include "sparta/simulation/ResourceFactory.hpp"

namespace pegasus
{
    class PegasusSystem;

    class PegasusCore : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "PegasusCore";

        class PegasusCoreParameters : public sparta::ParameterSet
        {
          public:
            PegasusCoreParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            PARAMETER(uint32_t, core_id, 0, "Core ID")
            PARAMETER(uint32_t, num_harts, 1, "Number of harts (hardware threads)")
        };

        PegasusCore(sparta::TreeNode* core_node, const PegasusCoreParameters* p);

        // Not default -- defined in source file to reduce massive inlining
        virtual ~PegasusCore();

        void boot();

        CoreId getCoreId() const { return core_id_; }

        uint32_t getNumThreads() const { return num_harts_; }

        PegasusState* getPegasusState(HartId hart_idx = 0) const { return threads_.at(hart_idx); }

        std::map<HartId, PegasusState*> & getThreads() { return threads_; }

        void stopSim(const int64_t exit_code);

      private:
        const CoreId core_id_;
        const uint32_t num_harts_;

        void onBindTreeLate_() override;

        sparta::ResourceFactory<pegasus::PegasusState,
                                pegasus::PegasusState::PegasusStateParameters>
            state_factory_;
        sparta::ResourceFactory<pegasus::Fetch, pegasus::Fetch::FetchParameters> fetch_factory_;
        sparta::ResourceFactory<pegasus::Translate, pegasus::Translate::TranslateParameters>
            translate_factory_;
        sparta::ResourceFactory<pegasus::Execute, pegasus::Execute::ExecuteParameters>
            execute_factory_;
        sparta::ResourceFactory<pegasus::Exception, pegasus::Exception::ExceptionParameters>
            exception_factory_;
        std::vector<std::unique_ptr<sparta::TreeNode>> tns_to_delete_;

        // Pegasus State for each hart
        std::map<HartId, PegasusState*> threads_;

        // Pegasus system
        PegasusSystem* system_ = nullptr;
    };
} // namespace pegasus
