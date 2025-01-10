#pragma once

#include "core/ActionGroup.hpp"
#include "core/AtlasTranslationState.hpp"
#include "core/observers/InstructionLogger.hpp"
#include "arch/RegisterSet.hpp"
#include "include/AtlasTypes.hpp"

#include "core/AtlasAllocatorWrapper.hpp"
#include "sim/AtlasAllocators.hpp"
#include "mavis/mavis/extension_managers/RISCVExtensionManager.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/Unit.hpp"
#include "core/AtlasInst.hpp"
#include "sparta/utils/SpartaSharedPointerAllocator.hpp"
#include "include/CSRBitMasks64.hpp"

#ifndef REG32_JSON_DIR
#error "REG32_JSON_DIR must be defined"
#endif

#ifndef REG64_JSON_DIR
#error "REG64_JSON_DIR must be defined"
#endif

template <class InstT, class ExtenT, class InstTypeAllocator, class ExtTypeAllocator> class Mavis;

namespace atlas
{
    class AtlasInst;
    using AtlasInstPtr = sparta::SpartaSharedPointer<AtlasInst>;
    class AtlasSystem;
    class Fetch;
    class Execute;
    class Translate;

    using MavisType =
        Mavis<AtlasInst, AtlasExtractor, AtlasInstAllocatorWrapper<AtlasInstAllocator>,
              AtlasExtractorAllocatorWrapper<AtlasExtractorAllocator>>;

    class AtlasState : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "AtlasState";
        using base_type = AtlasState;

        class AtlasStateParameters : public sparta::ParameterSet
        {
          public:
            AtlasStateParameters(sparta::TreeNode* node) : sparta::ParameterSet(node) {}

            PARAMETER(uint32_t, hart_id, 0, "Hart ID")
            PARAMETER(std::string, isa_string, "rv64g", "ISA string")
            PARAMETER(std::string, isa_file_path, "mavis_json", "Where are the Mavis isa files?")
            PARAMETER(std::string, uarch_file_path, "arch", "Where are the Atlas uarch files?")
            PARAMETER(bool, stop_sim_on_wfi, false, "Executing a WFI instruction stops simulation")
        };

        AtlasState(sparta::TreeNode* core_node, const AtlasStateParameters* p);

        // Not default -- defined in source file to reduce massive inlining
        ~AtlasState();

        HartId getHartId() const { return hart_id_; }

        uint64_t getXlen() const { return xlen_; }

        MavisType* getMavis() { return mavis_.get(); }

        bool getStopSimOnWfi() const { return stop_sim_on_wfi_; }

        void setPc(Addr pc) { pc_ = pc; }

        Addr getPc() const { return pc_; }

        void setNextPc(Addr next_pc) { next_pc_ = next_pc; }

        Addr getNextPc() const { return next_pc_; }

        void setPrivMode(PrivMode priv_mode) { priv_mode_ = priv_mode; }

        PrivMode getPrivMode() const { return priv_mode_; }

        void setNextPrivMode(PrivMode next_priv_mode) { next_priv_mode_ = next_priv_mode; }

        PrivMode getNextPrivMode() const { return next_priv_mode_; }

        uint64_t assignUid() { return uid_++; }

        struct SimState
        {
            AtlasInstPtr current_inst = nullptr;
            uint64_t inst_count = 0;
            bool sim_stopped = false;
            bool test_passed = true;
            uint64_t workload_exit_code = 0;

            void reset() { current_inst.reset(); }
        };

        const SimState* getSimState() const { return &sim_state_; }

        SimState* getSimState() { return &sim_state_; }

        const AtlasInstPtr & getCurrentInst() { return sim_state_.current_inst; }

        void setCurrentInst(AtlasInstPtr inst) { sim_state_.current_inst = inst; }

        AtlasTranslationState* getTranslationState() { return &translation_state_; }

        AtlasSystem* getAtlasSystem() const { return atlas_system_; }

        void setAtlasSystem(AtlasSystem* atlas_system) { atlas_system_ = atlas_system; }

        Fetch* getFetchUnit() const { return fetch_unit_; }

        Execute* getExecuteUnit() const { return execute_unit_; }

        Translate* getTranslateUnit() const { return translate_unit_; }

        atlas::RegisterSet* getIntRegisterSet() { return int_rset_.get(); }

        atlas::RegisterSet* getFpRegisterSet() { return fp_rset_.get(); }

        atlas::RegisterSet* getVecRegisterSet() { return vec_rset_.get(); }

        atlas::RegisterSet* getCsrRegisterSet() { return csr_rset_.get(); }

        sparta::Register* getIntRegister(uint32_t reg_num)
        {
            return int_rset_->getRegister(reg_num);
        }

        sparta::Register* getFpRegister(uint32_t reg_num) { return fp_rset_->getRegister(reg_num); }

        sparta::Register* getVecRegister(uint32_t reg_num)
        {
            return vec_rset_->getRegister(reg_num);
        }

        sparta::Register* getCsrRegister(uint32_t reg_num)
        {
            return csr_rset_->getRegister(reg_num);
        }

        template <typename MemoryType> MemoryType readMemory(const Addr paddr);

        template <typename MemoryType> void writeMemory(const Addr paddr, const MemoryType value);

        void addObserver(Observer* observer) { observers_.push_back(observer); }

        void insertExecuteActions(ActionGroup* action_group);

        ActionGroup* getStopSimActionGroup() { return &stop_sim_action_group_; }

      private:
        void onBindTreeEarly_() override;

        //! Hart ID
        const HartId hart_id_;

        // ISA string
        const std::string isa_string_;

        // Path to Mavis
        const std::string isa_file_path_;

        // Path to Atlas
        const std::string uarch_file_path_;

        // Mavis extension manager
        mavis::extension_manager::riscv::RISCVExtensionManager extension_manager_;

        // Mavis
        std::unique_ptr<MavisType> mavis_;

        //! Stop simulatiion on WFI
        const bool stop_sim_on_wfi_;

        // XLEN (either 32 or 64 bit)
        uint64_t xlen_ = 64;

        //! Current pc
        Addr pc_ = 0x0;

        //! Next pc
        Addr next_pc_ = 0x0;

        //! Current privilege mode
        PrivMode priv_mode_ = PrivMode::MACHINE;

        //! Next privilege mode
        PrivMode next_priv_mode_ = PrivMode::INVALID;

        //! Unique instruction ID
        uint64_t uid_ = 0;

        //! Simulation state
        SimState sim_state_;

        // Increment PC Action
        ActionGroup* incrementPc_(AtlasState* state);
        atlas::Action increment_pc_action_;

        // Translation state
        AtlasTranslationState translation_state_;

        //! AtlasSystem for accessing memory
        AtlasSystem* atlas_system_;

        // Fetch Unit
        Fetch* fetch_unit_ = nullptr;

        // Execute Unit
        Execute* execute_unit_ = nullptr;

        // Translate Unit
        Translate* translate_unit_ = nullptr;

        // Register set holding all Sparta registers from all generated JSON files
        std::unique_ptr<RegisterSet> int_rset_;
        std::unique_ptr<RegisterSet> fp_rset_;
        std::unique_ptr<RegisterSet> vec_rset_;
        std::unique_ptr<RegisterSet> csr_rset_;

        // Instruction Logger
        InstructionLogger inst_logger_;

        // Observers
        std::vector<Observer*> observers_;

        // Stop simulation Action
        Action stop_action_;
        ActionGroup stop_sim_action_group_;
    };
} // namespace atlas
