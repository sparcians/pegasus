#pragma once

#include "sparta/simulation/TreeNodeExtensions.hpp"
#include "sparta/simulation/ParameterSet.hpp"

#include <vector>
#include <string>

namespace pegasus
{
    // \class PegasusSimParameters
    // \brief TODO
    class PegasusSimParameters : public sparta::ExtensionsParamsOnly
    {
      public:
        static constexpr char NAME[] = "sim";

        using WorkloadAndArgs = std::vector<std::string>;
        using WorkloadsAndArgs = std::vector<WorkloadAndArgs>;
        using WorkloadsParam = sparta::Parameter<WorkloadsAndArgs>;

        using BinaryWithLoadAddr = std::vector<std::string>;
        using Binaries = std::vector<BinaryWithLoadAddr>;
        using LoadBinaryParam = sparta::Parameter<Binaries>;

        using RegisterOverride = std::vector<std::string>;
        using RegisterOverrides = std::vector<RegisterOverride>;
        using RegisterOverridesParam = sparta::Parameter<RegisterOverrides>;

        PegasusSimParameters() : sparta::ExtensionsParamsOnly() {}

        virtual ~PegasusSimParameters() {}

        void postCreate() override
        {
            sparta::ParameterSet* ps = getParameters();

            num_cores_.reset(
                new sparta::Parameter<uint32_t>("num_cores", 1, "Number of cores", ps));
            workloads_.reset(
                new WorkloadsParam("workloads", {}, "Workload(s) to run with arguments", ps));
            load_binaries_.reset(new LoadBinaryParam(
                "load_binaries", {}, "Binaries to load into memory at a specific address", ps));
            inst_limit_.reset(new sparta::Parameter<uint64_t>(
                "inst_limit", 0, "Instruction limit for all harts", ps));
            syscall_emulation_.reset(new sparta::Parameter<bool>(
                "enable_syscall_emulation", false, "System calls (ecall) will be emulated", ps));
            reg_overrides_.reset(new RegisterOverridesParam(
                "reg_overrides", {},
                "Override initial values of registers e.g. \"core0.hart0.sp 0x1000\"", ps));
        }

        template <typename T>
        static T getParameter(sparta::TreeNode* node, const std::string & param)
        {
            auto ext =
                sparta::notNull(node->getRoot()->createExtension(PegasusSimParameters::NAME));
            return ext->getParameterValueAs<T>(param);
        }

        template <typename T>
        static const std::string convertVectorToStringParam(const std::vector<T> vector_param)
        {
            std::string string_param = "[";
            for (uint32_t param_idx = 0; param_idx < vector_param.size(); ++param_idx)
            {
                if constexpr (sparta::utils::is_vector<T>::value)
                {
                    string_param += convertVectorToStringParam(vector_param[param_idx]);
                }
                else
                {
                    string_param += vector_param[param_idx];
                }
                const bool last_param = param_idx == (vector_param.size() - 1);
                string_param += last_param ? "" : ",";
            }
            string_param += "]";
            return string_param;
        }

      private:
        std::unique_ptr<sparta::Parameter<uint32_t>> num_cores_;
        std::unique_ptr<WorkloadsParam> workloads_;
        std::unique_ptr<LoadBinaryParam> load_binaries_;
        std::unique_ptr<sparta::Parameter<uint64_t>> inst_limit_;
        std::unique_ptr<sparta::Parameter<bool>> syscall_emulation_;
        std::unique_ptr<RegisterOverridesParam> reg_overrides_;
    };
} // namespace pegasus
