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

        using RegisterOverrides = std::vector<std::vector<std::string>>;
        using RegisterOverridesParam = sparta::Parameter<RegisterOverrides>;

        PegasusSimParameters() : sparta::ExtensionsParamsOnly() {}

        virtual ~PegasusSimParameters() {}

        void postCreate() override
        {
            sparta::ParameterSet* ps = getParameters();

            workloads_.reset(new WorkloadsParam("workloads", {}, "Workload(s) to run with arguments", ps));
            inst_limit_.reset(new sparta::Parameter<uint64_t>(
                "inst_limit", 0, "Instruction limit for all harts", ps));
            reg_overrides_.reset(new RegisterOverridesParam(
                "reg_overrides", {}, "Override initial values of registers e.g. \"core0.hart0.sp 0x1000\"", ps));
        }

      private:
        std::unique_ptr<WorkloadsParam> workloads_;
        std::unique_ptr<sparta::Parameter<uint64_t>> inst_limit_;
        std::unique_ptr<RegisterOverridesParam> reg_overrides_;
    };

} // namespace pegasus
