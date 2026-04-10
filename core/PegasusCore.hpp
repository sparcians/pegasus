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

#include "mavis/extension_managers/RISCVExtensionManager.hpp"

#include "sparta/simulation/ResourceFactory.hpp"
#include "sparta/events/Event.hpp"
#include "sparta/events/PayloadEvent.hpp"

namespace sparta::memory
{
    class BlockingMemoryIF;
} // namespace sparta::memory

namespace pegasus
{
    class PegasusSystem;
    class ReservationMemory;

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
            PARAMETER(std::string, arch, "default", "Architecture name")
            PARAMETER(std::string, profile, "", "RISC-V profile (defined in Mavis)")
            PARAMETER(std::string, isa, std::string("rv64") + DEFAULT_ISA_STR, "ISA string")
            PARAMETER(std::string, priv, "msu", "Privilege modes supported")
            PARAMETER(std::string, isa_file_path, "mavis_json", "Where are the Mavis isa files?")
            PARAMETER(std::string, uarch_file_path, "arch", "Where are the Pegasus uarch files?")
            PARAMETER(uint64_t, pause_counter_duration, 256, "Pause counter duration in cycles")
            PARAMETER(uint64_t, wrssto_counter_duration, 256,
                      "WRS.STO pause counter duration in cycles")
            PARAMETER(std::vector<int>, supported_trap_modes, {0},
                      "Supported RISC-V trap modes (0: Direct, 1: Vectored)")
            PARAMETER(bool, misalignment_support, true, "Misalignment Support");

            HIDDEN_PARAMETER(bool, cosim_mode, false, "Set by PegasusCoSim");
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

        sparta::memory::BlockingMemoryIF* getMemory() const { return current_memory_view_; }

        SystemCallEmulator* getSystemCallEmulator() const { return system_call_emulator_; }

        bool inCoSimMode() const { return cosim_mode_; }

        bool isSystemCallEmulationEnabled() const { return syscall_emulation_enabled_; }

        bool isPrivilegeModeSupported(const PrivMode mode) const
        {
            return supported_priv_modes_.contains(mode);
        }

        bool isTrapModeSupported(const TrapVectorMode mode) const
        {
            const auto & modes = supported_trap_modes_;
            return std::find(modes.begin(), modes.end(), static_cast<int>(mode)) != modes.end();
        }

        uint64_t getXlen() const { return xlen_; }

        bool isMisalignmentSupported() const { return misalignment_support_; }

        // Is the "H" extension enabled?
        bool hasHypervisor() const { return hypervisor_enabled_; }

        template <typename XLEN> uint32_t getMisaExtFieldValue() const;

        using Reservation = sparta::utils::ValidValue<Addr>;

        void makeReservation(HartId hart_id, Addr paddr);

        Reservation & getReservation(HartId hart_id) { return reservations_.at(hart_id); }

        const Reservation & getReservation(HartId hart_id) const
        {
            return reservations_.at(hart_id);
        }

        void clearReservation(HartId hart_id);

        const InstHandlers* getInstHandlers() const { return &inst_handlers_; }

        const std::string & getISAString() const { return isa_string_; }

        void unpauseHart(HartId hart_id) { threads_running_.set(hart_id); }

        void cancelWrsstoEvent(HartId hart_id) { ev_wrssto_counter_expires_.cancelIf(hart_id); }

        template <bool IS_UNIT_TEST = false> bool compare(const PegasusCore* core) const;

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

        // Execute the threads on this core
        void advanceSim_();
        sparta::Event<> ev_advance_sim_;

        // Pause counter
        const uint64_t pause_counter_duration_;
        const uint64_t wrssto_counter_duration_;
        void pauseCounterExpires_(const HartId & hart_id);
        sparta::PayloadEvent<HartId> ev_pause_counter_expires_;
        void wrsstoCounterExpires_(const HartId & hart_id);
        sparta::PayloadEvent<HartId> ev_wrssto_counter_expires_;

        // Status of each thread
        HartId current_hart_id_ = 0;
        std::bitset<8> threads_running_;

        // Is this a PegasusCoSim run?
        const bool cosim_mode_;

        // Is system call emulation enabled?
        const bool syscall_emulation_enabled_;

        // Arch name
        // FIXME: We should be able to get this from Sparta --arch
        const std::string arch_name_;

        // RISC-V profile
        const std::string profile_;

        // ISA string
        const std::string isa_string_;

        // Supported privilege modes
        const std::unordered_set<PrivMode> supported_priv_modes_;

        // XLEN (either 32 or 64 bit)
        uint64_t xlen_ = 64;

        // Supported ISA string
        const std::vector<std::string> supported_rv64_extensions_;
        const std::vector<std::string> supported_rv32_extensions_;

        // Supported Trap Modes
        const std::vector<int> supported_trap_modes_;

        // Does the config support address misalignment
        const bool misalignment_support_;

        // Path to Mavis isa JSONs
        const std::string isa_file_path_;

        // Path to Pegasus uarch JSONs
        const std::string uarch_file_path_;

        // Mavis extension manager
        mavis::extension_manager::riscv::RISCVExtensionManager extension_manager_;

        inline bool validateISAString_(std::string & unsupportedExt);

        //! Do we have hypervisor?
        const bool hypervisor_enabled_;

        //! LR/SC Reservations
        std::vector<Reservation> reservations_;

        // Instruction Actions
        InstHandlers inst_handlers_;

        // Current active BlockingMemoryIF for this core
        sparta::memory::BlockingMemoryIF* current_memory_view_ = nullptr;

        // ReservationMemory
        std::unique_ptr<ReservationMemory> reservation_memory_bmi_;
        ;
    };
} // namespace pegasus
