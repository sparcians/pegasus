#pragma once

#include "sparta/simulation/TreeNodeExtensions.hpp"
#include "sparta/simulation/ParameterSet.hpp"

#include <vector>
#include <string>

namespace pegasus
{

    //
    // \class PegasusSimParameters
    // \brief TODO
    class PegasusSimParameters : public sparta::ExtensionsParamsOnly
    {
      public:
        static constexpr char name[] = "sim";

        using WorkloadAndArgs = std::vector<std::string>;
        using WorkloadsAndArgs = std::vector<WorkloadAndArgs>;
        using WorkloadsParam = sparta::Parameter<WorkloadsAndArgs>;

        using RegValueOverridePairs = std::vector<std::pair<std::string, std::string>>;

        PegasusSimParameters() : sparta::ExtensionsParamsOnly() {}

        virtual ~PegasusSimParameters() {}

        void postCreate() override
        {
            sparta::ParameterSet* ps = getParameters();

            workloads_.reset(
                new WorkloadsParam("workloads", {}, "Workload(s) to run", ps));
            inst_limit_.reset(
                new sparta::Parameter<uint64_t>("inst_limit", 0, "Instruction limit for all harts", ps));
        }

      private:
        std::unique_ptr<WorkloadsParam> workloads_;
        std::unique_ptr<sparta::Parameter<uint64_t>> inst_limit_;
    };

} // namespace pegasus
