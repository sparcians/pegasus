#include "core/InstHandlers.hpp"

#include "core/inst_handlers/a/RvaInsts.hpp"
#include "core/inst_handlers/b/RvzbaInsts.hpp"
#include "core/inst_handlers/b/RvzbbInsts.hpp"
#include "core/inst_handlers/b/RvzbcInsts.hpp"
#include "core/inst_handlers/b/RvzbkbInsts.hpp"
#include "core/inst_handlers/b/RvzbsInsts.hpp"
#include "core/inst_handlers/d/RvdInsts.hpp"
#include "core/inst_handlers/f/RvfInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "core/inst_handlers/m/RvmInsts.hpp"
#include "core/inst_handlers/zicsr/RvzicsrInsts.hpp"
#include "core/inst_handlers/zifencei/RvzifenceiInsts.hpp"
#include "core/inst_handlers/zihintpause/RvzihintpauseInsts.hpp"
#include "core/inst_handlers/zicond/RvzicondInsts.hpp"
#include "core/inst_handlers/zcmp/RvzcmpInsts.hpp"
#include "core/inst_handlers/zcmt/RvzcmtInsts.hpp"
#include "core/inst_handlers/zabha/RvzabhaInsts.hpp"
#include "core/inst_handlers/zilsd/RvzilsdInsts.hpp"
#include "core/inst_handlers/zfa/RvzfaInsts.hpp"
#include "core/inst_handlers/v/RvvConfigSettingInsts.hpp"
#include "core/inst_handlers/v/RvvIntegerInsts.hpp"
#include "core/inst_handlers/v/RvvLoadStoreInsts.hpp"
#include "core/inst_handlers/v/RvvMaskInsts.hpp"
#include "core/inst_handlers/v/RvvReductionInsts.hpp"
#include "core/inst_handlers/v/RvvFloatInsts.hpp"
#include "core/inst_handlers/v/RvvPermuteInsts.hpp"
#include "core/inst_handlers/v/RvvFixedPointInsts.hpp"
#include "core/inst_handlers/zfh/RvzfhInsts.hpp"

namespace pegasus
{
    InstHandlers::InstHandlers(const bool enable_syscall_emulation)
    {
        // Handler for unsupported instructions
        rv64_inst_actions_.emplace(
            "unsupported",
            pegasus::Action::createAction<&InstHandlers::unsupportedInstHandler_, InstHandlers>(
                nullptr, "unsupported", ActionTags::EXECUTE_TAG));
        rv32_inst_actions_.emplace(
            "unsupported",
            pegasus::Action::createAction<&InstHandlers::unsupportedInstHandler_, InstHandlers>(
                nullptr, "unsupported", ActionTags::EXECUTE_TAG));
        rv64_inst_compute_address_actions_.emplace(
            "unsupported",
            pegasus::Action::createAction<&InstHandlers::unsupportedInstHandler_, InstHandlers>(
                nullptr, "unsupported", ActionTags::COMPUTE_ADDR_TAG));
        rv32_inst_compute_address_actions_.emplace(
            "unsupported",
            pegasus::Action::createAction<&InstHandlers::unsupportedInstHandler_, InstHandlers>(
                nullptr, "unsupported", ActionTags::COMPUTE_ADDR_TAG));

        // Get RV64 instruction handlers
        RvzbaInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzbbInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzbcInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzbkbInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzbsInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RviInsts::getInstHandlers<RV64>(rv64_inst_actions_, enable_syscall_emulation);
        RvmInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvaInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvfInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvdInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzicsrInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzifenceiInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzihintpauseInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzicondInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzcmpInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzcmtInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzabhaInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzfaInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzfhInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        // Vector
        RvvConfigSettingInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvIntegerInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvLoadStoreInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvMaskInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvReductionInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvFloatInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvPermuteInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvFixedPointInsts::getInstHandlers<RV64>(rv64_inst_actions_);

        // Get RV32 instruction handlers
        RvzbaInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzbbInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzbcInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzbkbInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzbsInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RviInsts::getInstHandlers<RV32>(rv32_inst_actions_, enable_syscall_emulation);
        RvmInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvaInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvfInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvdInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzicsrInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzifenceiInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzihintpauseInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzicondInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzcmpInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzcmtInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzabhaInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzfaInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzfhInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        // Vector
        RvvConfigSettingInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvIntegerInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvLoadStoreInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvMaskInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvReductionInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvFloatInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvPermuteInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvFixedPointInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        // RV32 only
        RvzilsdInsts::getInstHandlers<RV32>(rv32_inst_actions_);

        // Get RV64 instruction compute address handlers
        RviInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvaInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvfInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvdInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvzcmtInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvzabhaInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvvLoadStoreInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvzfhInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);

        // Get RV32 instruction compute address handlers
        RviInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvaInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvfInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvdInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvzcmtInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvzabhaInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvvLoadStoreInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvzfhInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        // RV32 only
        RvzilsdInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);

        // Get CSR update handlers
        RvzicsrInsts::getCsrUpdateActions<RV64>(rv64_csr_update_actions_);
        RvzicsrInsts::getCsrUpdateActions<RV32>(rv32_csr_update_actions_);
    }

    template const InstHandlers::InstHandlersMap* InstHandlers::getInstHandlersMap<RV64>() const;
    template const InstHandlers::InstHandlersMap* InstHandlers::getInstHandlersMap<RV32>() const;
    template const InstHandlers::InstHandlersMap*
    InstHandlers::getInstComputeAddressHandlersMap<RV64>() const;
    template const InstHandlers::InstHandlersMap*
    InstHandlers::getInstComputeAddressHandlersMap<RV32>() const;

    Action::ItrType InstHandlers::unsupportedInstHandler_(pegasus::PegasusState* state,
                                                          Action::ItrType action_it)
    {
        sparta_assert(false,
                      "Failed to execute. Instruction is unsupported: " << state->getCurrentInst());
        return ++action_it;
    }
} // namespace pegasus
