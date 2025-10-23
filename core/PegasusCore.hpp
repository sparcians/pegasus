#pragma once

#include <vector>
#include <string>
#include <cinttypes>

#include "core/PegasusAllocatorWrapper.hpp"
#include "core/PegasusState.hpp"
#include "core/InstHandlers.hpp"
#include "core/Fetch.hpp"
#include "core/translate/Translate.hpp"
#include "core/Execute.hpp"
#include "core/Exception.hpp"

#include "mavis/mavis/extension_managers/RISCVExtensionManager.hpp"

#include "sparta/simulation/ResourceFactory.hpp"

template <class InstT, class ExtenT, class InstTypeAllocator, class ExtTypeAllocator> class Mavis;

namespace pegasus
{
    class PegasusSystem;

    using MavisType =
        Mavis<PegasusInst, PegasusExtractor, PegasusInstAllocatorWrapper<PegasusInstAllocator>,
              PegasusExtractorAllocatorWrapper<PegasusExtractorAllocator>>;

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
            PARAMETER(bool, enable_syscall_emulation, false,
                      "System calls (ecall) will be emulated");
            PARAMETER(std::string, isa, std::string("rv64") + DEFAULT_ISA_STR, "ISA string")
            PARAMETER(std::string, priv, "msu", "Privilege modes supported")
            PARAMETER(std::string, isa_file_path, "mavis_json", "Where are the Mavis isa files?")
            PARAMETER(std::string, uarch_file_path, "arch", "Where are the Pegasus uarch files?")
        };

        PegasusCore(sparta::TreeNode* core_node, const PegasusCoreParameters* p);

        // Not default -- defined in source file to reduce massive inlining
        virtual ~PegasusCore();

        void boot();

        void stopSim(const int64_t exit_code);

        CoreId getCoreId() const { return core_id_; }

        uint32_t getNumThreads() const { return num_harts_; }

        PegasusState* getPegasusState(HartId hart_idx = 0) const { return threads_.at(hart_idx); }

        std::map<HartId, PegasusState*> & getThreads() { return threads_; }

        PegasusSystem* getSystem() const { return system_; }

        SystemCallEmulator* getSystemCallEmulator() const { return system_call_emulator_; }

        bool isSystemCallEmulationEnabled() const { return syscall_emulation_enabled_; }

        bool isPrivilegeModeSupported(const PrivMode mode) const
        {
            return supported_priv_modes_.contains(mode);
        }

        uint64_t getXlen() const { return xlen_; }

        mavis::extension_manager::riscv::RISCVExtensionManager & getExtensionManager()
        {
            return extension_manager_;
        }

        bool isExtensionEnabled(std::string ext) const { return extension_manager_.isEnabled(ext); }

        bool isCompressionEnabled() const { return extension_manager_.isEnabled("zca"); }

        // Is the "H" extension enabled?
        bool hasHypervisor() const { return hypervisor_enabled_; }

        MavisType* getMavis() { return mavis_.get(); }

        enum MavisUIDs : mavis::InstructionUniqueID
        {
            MAVIS_UID_CSRRW = 1,
            MAVIS_UID_CSRRS,
            MAVIS_UID_CSRRC,
            MAVIS_UID_CSRRWI,
            MAVIS_UID_CSRRSI,
            MAVIS_UID_CSRRCI
        };

        void changeMavisContext();

        template <typename XLEN> uint32_t getMisaExtFieldValue() const;

        uint64_t getPcAlignment() const { return pc_alignment_; }

        uint64_t getPcAlignmentMask() const { return pc_alignment_mask_; }

        const InstHandlers* getInstHandlers() const { return &inst_handlers_; }

      private:
        void onBindTreeEarly_() override;
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

        // Pegasus system
        PegasusSystem* system_ = nullptr;

        //! System Call Emulator for ecall emulation
        SystemCallEmulator* system_call_emulator_ = nullptr;

        // Core ID
        const CoreId core_id_;

        // Number of hardware threads on this core
        const uint32_t num_harts_;

        // Pegasus State for each hart
        std::map<HartId, PegasusState*> threads_;

        // Is system call emulation enabled?
        const bool syscall_emulation_enabled_;

        // ISA string
        const std::string isa_string_;

        // Supported privilege modes
        const std::unordered_set<PrivMode> supported_priv_modes_;

        // XLEN (either 32 or 64 bit)
        uint64_t xlen_ = 64;

        // Supported ISA string
        const std::vector<std::string> supported_rv64_extensions_;
        const std::vector<std::string> supported_rv32_extensions_;

        // Path to Mavis
        const std::string isa_file_path_;

        // Path to Pegasus
        const std::string uarch_file_path_;

        // Get Pegasus arch JSONs for Mavis
        mavis::FileNameListType getUArchFiles_() const;

        // Mavis extension manager
        mavis::extension_manager::riscv::RISCVExtensionManager extension_manager_;

        // Mavis
        std::unique_ptr<MavisType> mavis_;

        static inline mavis::InstUIDList mavis_uid_list_{
            {"csrrw", MAVIS_UID_CSRRW},   {"csrrs", MAVIS_UID_CSRRS},
            {"csrrc", MAVIS_UID_CSRRC},   {"csrrwi", MAVIS_UID_CSRRWI},
            {"csrrsi", MAVIS_UID_CSRRSI}, {"csrrci", MAVIS_UID_CSRRCI}};

        inline bool validateISAString_(std::string & unsupportedExt);

        //! Do we have hypervisor?
        const bool hypervisor_enabled_;

        //! PC alignment
        uint64_t pc_alignment_ = 4;

        //! PC alignment
        uint64_t pc_alignment_mask_ = ~(pc_alignment_ - 1);

        void setPcAlignment_(uint64_t pc_alignment)
        {
            sparta_assert(pc_alignment == 2 || pc_alignment == 4,
                          "Invalid PC alignment value! " << pc_alignment);
            pc_alignment_ = pc_alignment;
            pc_alignment_mask_ = ~(pc_alignment - 1);
        }

        // Instruction Actions
        InstHandlers inst_handlers_;
    };
} // namespace pegasus
