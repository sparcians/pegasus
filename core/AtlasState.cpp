#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/Fetch.hpp"
#include "core/Execute.hpp"
#include "core/Translate.hpp"
#include "core/Exception.hpp"
#include "include/ActionTags.hpp"
#include "include/AtlasUtils.hpp"
#include "system/AtlasSystem.hpp"

#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "arch/register_macros.hpp"
#include "include/AtlasTypes.hpp"

namespace atlas
{
    AtlasState::AtlasState(sparta::TreeNode* core_node, const AtlasStateParameters* p) :
        sparta::Unit(core_node),
        hart_id_(p->hart_id),
        stop_sim_on_wfi_(p->stop_sim_on_wfi),
        inst_logger_(core_node),
        stop_sim_action_group_("stop_sim")
    {
        auto json_dir = (xlen_ == 32) ? REG32_JSON_DIR : REG64_JSON_DIR;
        int_rset_ =
            RegisterSet::create(core_node, json_dir + std::string("/reg_int.json"), "int_regs");
        fp_rset_ =
            RegisterSet::create(core_node, json_dir + std::string("/reg_fp.json"), "fp_regs");
        vec_rset_ =
            RegisterSet::create(core_node, json_dir + std::string("/reg_vec.json"), "vec_regs");
        csr_rset_ =
            RegisterSet::create(core_node, json_dir + std::string("/reg_csr.json"), "csr_regs");

        // Increment PC Action
        increment_pc_action_ =
            atlas::Action::createAction<&AtlasState::incrementPc_>(this, "increment pc");

        // Create Action to stop simulation
        stop_action_.addTag(ActionTags::STOP_SIM_TAG);
        stop_sim_action_group_.addAction(stop_action_);
    }

    // Not default -- defined in source file to reduce massive inlining
    AtlasState::~AtlasState() {}

    void AtlasState::onBindTreeEarly_()
    {
        fetch_unit_ = getContainer()->getChild("fetch")->getResourceAs<Fetch*>();
        execute_unit_ = getContainer()->getChild("execute")->getResourceAs<Execute*>();
        translate_unit_ = getContainer()->getChild("translate")->getResourceAs<Translate*>();
        exception_unit_ = getContainer()->getChild("exception")->getResourceAs<Exception*>();
    }

    template <typename MemoryType> MemoryType AtlasState::readMemory(const Addr paddr)
    {
        auto* memory = atlas_system_->getSystemMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        std::vector<uint8_t> buffer(sizeof(MemoryType) / sizeof(uint8_t), 0);
        const bool success = memory->tryRead(paddr, size, buffer.data());
        sparta_assert(success, "Failed to read from memory at address 0x" << std::hex << paddr);

        const MemoryType value = convertFromByteVector<MemoryType>(buffer);
        ILOG("Memory read to 0x" << std::hex << paddr << ": 0x" << (uint64_t)value);
        return value;
    }

    template <typename MemoryType>
    void AtlasState::writeMemory(const Addr paddr, const MemoryType value)
    {
        auto* memory = atlas_system_->getSystemMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        const std::vector<uint8_t> buffer = convertToByteVector<MemoryType>(value);
        const bool success = memory->tryWrite(paddr, size, buffer.data());
        sparta_assert(success, "Failed to write to memory at address 0x" << std::hex << paddr);

        ILOG("Memory write to 0x" << std::hex << paddr << ": 0x" << (uint64_t)value);
    }

    template int8_t AtlasState::readMemory<int8_t>(const Addr);
    template uint8_t AtlasState::readMemory<uint8_t>(const Addr);
    template int16_t AtlasState::readMemory<int16_t>(const Addr);
    template uint16_t AtlasState::readMemory<uint16_t>(const Addr);
    template int32_t AtlasState::readMemory<int32_t>(const Addr);
    template uint32_t AtlasState::readMemory<uint32_t>(const Addr);
    template int64_t AtlasState::readMemory<int64_t>(const Addr);
    template uint64_t AtlasState::readMemory<uint64_t>(const Addr);
    template void AtlasState::writeMemory<uint8_t>(const Addr, const uint8_t);
    template void AtlasState::writeMemory<uint16_t>(const Addr, const uint16_t);
    template void AtlasState::writeMemory<uint32_t>(const Addr, const uint32_t);
    template void AtlasState::writeMemory<uint64_t>(const Addr, const uint64_t);

    void AtlasState::insertExecuteActions(ActionGroup* action_group)
    {
        // FIXME: InstructionLogger's enable method doesn't work when using a BaseObserver
        // pointer...
        if (inst_logger_.enabled())
        {
            inst_logger_.insertPreExecuteAction(action_group);
            inst_logger_.insertPostExecuteAction(action_group);
        }

        for (auto observer : observers_)
        {
            if (observer->enabled())
            {
                observer->insertPreExecuteAction(action_group);
                observer->insertPostExecuteAction(action_group);
            }
        }

        // Increment PC and upate simulation state
        action_group->insertActionAfter(increment_pc_action_, ActionTags::EXECUTE_TAG);
    }

    ActionGroup* AtlasState::incrementPc_(AtlasState*)
    {
        // Set PC
        pc_ = next_pc_;
        next_pc_ = 0;

        // Increment instruction count
        ++sim_state_.inst_count;

        return nullptr;
    }
} // namespace atlas
