#include "PegasusCore.hpp"
#include "system/PegasusSystem.hpp"
#include "system/SystemCallEmulator.hpp"
#include "include/gen/CSRBitMasks32.hpp"

#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "sparta/events/StartupEvent.hpp"
#include "sparta/utils/SpartaTester.hpp"

#include <filesystem>
#include <regex>

namespace pegasus
{
    std::unordered_set<PrivMode> initSupportedPrivilegeModes(const std::string & priv)
    {
        std::unordered_set<PrivMode> priv_modes;

        for (const char mode : priv)
        {
            if (mode == 'm')
            {
                priv_modes.emplace(PrivMode::MACHINE);
            }
            else if (mode == 's')
            {
                priv_modes.emplace(PrivMode::SUPERVISOR);
            }
            else if (mode == 'u')
            {
                priv_modes.emplace(PrivMode::USER);
            }
            else
            {
                sparta_assert(false, "Unsupported privilege modes: "
                                         << priv
                                         << "\nSupported privilege modes are: Machine (M), "
                                            "Supervisor (S) and User (U)");
            }
        }

        return priv_modes;
    }

    uint32_t getXlenFromIsaString(const std::string & isa_string)
    {
        if (isa_string.find("32") != std::string::npos)
        {
            return 32;
        }
        else if (isa_string.find("64") != std::string::npos)
        {
            return 64;
        }
        else
        {
            sparta_assert(false, "Failed to determine XLEN from ISA string: " << isa_string);
        }
    }

    bool PegasusCore::validateISAString_(std::string & unsupportedExt)
    {
        const auto & supported_exts =
            (xlen_ == 64) ? supported_rv64_extensions_ : supported_rv32_extensions_;

        const auto hasExtension = [&](const std::string & ext) {
            return std::find(supported_exts.begin(), supported_exts.end(), ext)
                   != supported_exts.end();
        };

        for (const auto & ext : extension_manager_.getEnabledExtensions(false))
        {
            if (!hasExtension(ext.first))
            {
                unsupportedExt = ext.first;
                return false;
            }
        }

        return true;
    }

    PegasusCore::PegasusCore(sparta::TreeNode* core_tn, const PegasusCoreParameters* p) :
        sparta::Unit(core_tn),
        core_id_(p->core_id),
        num_harts_(p->num_harts),
        ev_advance_sim_(&unit_event_set_, "advance_sim",
                        CREATE_SPARTA_HANDLER(PegasusCore, advanceSim_)),
        pause_counter_duration_(p->pause_counter_duration),
        wrssto_counter_duration_(p->wrssto_counter_duration),
        ev_pause_counter_expires_(
            &unit_event_set_, "pause_counter_expires",
            CREATE_SPARTA_HANDLER_WITH_DATA(PegasusCore, pauseCounterExpires_, HartId)),
        ev_wrssto_counter_expires_(
            &unit_event_set_, "wrssto_counter_expires",
            CREATE_SPARTA_HANDLER_WITH_DATA(PegasusCore, wrsstoCounterExpires_, HartId)),
        cosim_mode_(p->cosim_mode),
        syscall_emulation_enabled_(
            PegasusSimParameters::getParameter<bool>(core_tn, "enable_syscall_emulation")),
        arch_name_(p->arch),
        profile_(p->profile),
        isa_string_(p->isa),
        supported_priv_modes_(initSupportedPrivilegeModes(p->priv)),
        xlen_(getXlenFromIsaString(isa_string_)),
        supported_rv64_extensions_(SUPPORTED_RV64_EXTS),
        supported_rv32_extensions_(SUPPORTED_RV32_EXTS),
        supported_trap_modes_(p->supported_trap_modes),
        isa_file_path_(p->isa_file_path),
        uarch_file_path_(p->uarch_file_path),
        extension_manager_(mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            isa_string_, isa_file_path_ + std::string("/riscv_isa_spec.json"), isa_file_path_)),
        hypervisor_enabled_(extension_manager_.isEnabled("h")),
        reservations_(num_harts_),
        inst_handlers_(syscall_emulation_enabled_)
    {
        // top.core*.hart*
        for (HartId hart_idx = 0; hart_idx < num_harts_; ++hart_idx)
        {
            sparta::TreeNode* hart_tn = nullptr;
            const std::string hart_name = "hart" + std::to_string(hart_idx);
            tns_to_delete_.emplace_back(
                hart_tn = new sparta::ResourceTreeNode(core_tn, hart_name, "harts", hart_idx,
                                                       "Hart State", &state_factory_));

            // Set XLEN
            hart_tn->getChildAs<sparta::ParameterBase>("params.xlen")
                ->setValueFromString(std::to_string(xlen_));

            // Set hart_id if not explicitly set
            auto hart_id = hart_tn->getChildAs<sparta::ParameterBase>("params.hart_id");
            if (hart_id->isDefault())
            {
                hart_id->setValueFromString(std::to_string(hart_idx));
            }

            // Set path to register JSONs (from "arch")
            const std::string reg_json_file_path =
                uarch_file_path_ + "/" + arch_name_ + "/rv" + std::to_string(xlen_) + "/gen";
            hart_tn->getChildAs<sparta::ParameterBase>("params.reg_json_file_path")
                ->setValueFromString(reg_json_file_path);

            // top.core*.hart*.fetch
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "fetch", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Fetch Unit", &fetch_factory_));

            // top.core*.hart*.translate
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "translate", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Translate Unit", &translate_factory_));

            // top.core*.hart*.execute
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "execute", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Execute Unit", &execute_factory_));

            // top.core*.hart*.exception
            tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
                hart_tn, "exception", sparta::TreeNode::GROUP_NAME_NONE,
                sparta::TreeNode::GROUP_IDX_NONE, "Exception Unit", &exception_factory_));
        }

        sparta_assert(xlen_ == extension_manager_.getXLEN());

        if (profile_.empty() == false)
        {
            extension_manager_.setProfile(profile_);
        }
        extension_manager_.setISA(isa_string_);

        std::string unsupportedExt;
        if (!validateISAString_(unsupportedExt))
        {
            sparta_assert(false,
                          "ISA extension: " << unsupportedExt
                                            << " is not supported in isa_string: " << isa_string_);
        }

        sparta::StartupEvent(core_tn, CREATE_SPARTA_HANDLER(PegasusCore, advanceSim_));
    }

    PegasusCore::~PegasusCore() {}

    void PegasusCore::boot()
    {
        for (auto & [hart_idx, thread] : threads_)
        {
            thread->boot();
        }
    }

    void PegasusCore::stopSim(const int64_t exit_code)
    {
        DLOG("Stopping simulation for all harts");
        for (auto & [hart_idx, thread] : threads_)
        {
            thread->stopSim(exit_code);
            threads_running_.reset(hart_idx);
        }

        ev_pause_counter_expires_.cancel();
        ev_wrssto_counter_expires_.cancel();
    }

    void PegasusCore::onBindTreeEarly_()
    {
        // Pegasus System (shared by all harts)
        auto root_tn = getContainer()->getParentAs<sparta::RootTreeNode>();
        system_ = root_tn->getChild("system")->getResourceAs<pegasus::PegasusSystem>();

        for (uint32_t hart_idx = 0; hart_idx < num_harts_; ++hart_idx)
        {
            // Get PegasusState for each hart
            const std::string hart_name = "hart" + std::to_string(hart_idx);
            const auto thread = getContainer()->getChild(hart_name);
            threads_.emplace(hart_idx, thread->getResourceAs<PegasusState*>());

            PegasusState* state = threads_.at(hart_idx);
            // This MUST be done before initializing Mavis
            state->setPegasusCore(this);
        }

        // Initialize Mavis
        DLOG("Initializing Mavis with ISA string " << isa_string_);

        mavis_ = std::make_unique<MavisType>(
            extension_manager_.constructMavis<
                PegasusInst, PegasusExtractor, PegasusInstAllocatorWrapper<PegasusInstAllocator>,
                PegasusExtractorAllocatorWrapper<PegasusExtractorAllocator>>(
                getUArchFiles_(), mavis_uid_list_, {}, // annotation overrides
                {},                                    // inclusions
                {},                                    // exclusions
                PegasusInstAllocatorWrapper<PegasusInstAllocator>(
                    sparta::notNull(PegasusAllocators::getAllocators(getContainer()))
                        ->inst_allocator),
                PegasusExtractorAllocatorWrapper<PegasusExtractorAllocator>(
                    sparta::notNull(PegasusAllocators::getAllocators(getContainer()))
                        ->extractor_allocator,
                    this)));

        if (isCompressionEnabled())
        {
            setPcAlignment_(2);
        }
        else
        {
            setPcAlignment_(4);
        }
    }

    void PegasusCore::onBindTreeLate_()
    {
        auto root_tn = getContainer()->getParentAs<sparta::RootTreeNode>();
        system_call_emulator_ =
            root_tn->getChild("system_call_emulator")->getResourceAs<pegasus::SystemCallEmulator>();

        for (uint32_t hart_idx = 0; hart_idx < num_harts_; ++hart_idx)
        {
            PegasusState* state = threads_.at(hart_idx);

            const auto workloads_and_args = system_->getWorkloadsAndArgs();
            if (hart_idx < workloads_and_args.size())
            {
                state->setupProgramStack(workloads_and_args.at(hart_idx));

                state->setPc(system_->getStartingPc());
                state->getSimState()->sim_stopped = false;
                threads_running_.set(hart_idx);
            }
        }

        // FIXME: This is a workaround to get the "--opcode" command line option
        // to work, assume hart0 is always running at boot
        PegasusState* state = threads_.at(0);
        state->setPc(system_->getStartingPc());
        state->getSimState()->sim_stopped = false;
        threads_running_.set(0);
    }

    void PegasusCore::changeMavisContext()
    {
        extension_manager_.switchMavisContext(*mavis_.get());

        if (isCompressionEnabled())
        {
            setPcAlignment_(2);
        }
        else
        {
            setPcAlignment_(4);
        }
    }

    template <typename XLEN> uint32_t PegasusCore::getMisaExtFieldValue() const
    {
        uint32_t ext_val = 0;
        for (char ext = 'a'; ext <= 'z'; ++ext)
        {
            const std::string ext_str = std::string(1, ext);
            if (extension_manager_.isEnabled(ext_str))
            {
                ext_val |= 1 << getCsrBitRange<XLEN>(MISA, ext_str.c_str()).first;
            }
        }

        // FIXME: Assume both User and Supervisor mode are supported
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            ext_val |= 1 << CSR_64::MISA::u::high_bit;
            ext_val |= 1 << CSR_64::MISA::s::high_bit;
        }
        else
        {
            ext_val |= 1 << CSR_32::MISA::u::high_bit;
            ext_val |= 1 << CSR_32::MISA::s::high_bit;
        }

        return ext_val;
    }

    template uint32_t PegasusCore::getMisaExtFieldValue<RV32>() const;
    template uint32_t PegasusCore::getMisaExtFieldValue<RV64>() const;

    mavis::FileNameListType PegasusCore::getUArchFiles_() const
    {
        mavis::FileNameListType uarch_files;

        const std::string xlen_str = std::to_string(xlen_);
        const std::string xlen_uarch_file_path =
            uarch_file_path_ + "/" + arch_name_ + "/rv" + xlen_str + "/gen";
        const std::regex filename_regex("pegasus_uarch_.*json");
        for (const auto & entry : std::filesystem::directory_iterator{xlen_uarch_file_path})
        {
            if (std::regex_search(entry.path().filename().string(), filename_regex))
            {
                DLOG("Loading: " << entry.path());
                uarch_files.emplace_back(entry.path());
            }
        }

        return uarch_files;
    }

    // This method will execute a single thread for up to X instructions
    // where X is the quantum for that thread. The cycle count of all threads
    // is updated to match the current thread's cycle count. Then this event
    // is rescheduled at that cycle count.
    void PegasusCore::advanceSim_()
    {
        PegasusState* state = threads_[current_hart_id_];
        auto* sim_state = state->getSimState();
        bool timed_pause = false;
        if ((sim_state->sim_stopped == false)
            && (sim_state->sim_pause_reason == SimPauseReason::INVALID))
        {
            DLOG("Running hart" << std::dec << current_hart_id_);
            Fetch* fetch = state->getFetchUnit();
            ActionGroup* next_action_group = fetch->getActionGroup();
            while (next_action_group)
            {
                next_action_group = next_action_group->execute(state);
            }

            if (sim_state->sim_stopped)
            {
                DLOG("Stopping hart" << std::dec << current_hart_id_);
                threads_running_.reset(current_hart_id_);
            }

            switch (sim_state->sim_pause_reason)
            {
                case SimPauseReason::QUANTUM:
                    state->unpauseHart();
                    break;
                case SimPauseReason::INTERRUPT:
                    sparta_assert(false, "Pause reason INTERRUPT is not supported yet!");
                    break;
                case SimPauseReason::PAUSE:
                case SimPauseReason::WRS_STO:
                    timed_pause = true;
                    break;
                case SimPauseReason::FORK:
                    sparta_assert(false, "Pause reason FORK is not supported yet!");
                    break;
                case SimPauseReason::WRS_NTO:
                case SimPauseReason::INVALID:
                    break;
            }
        }

        // Update current cycle for all threads
        const uint64_t current_cycle = sim_state->cycles;
        for (HartId hart_id = 0; hart_id < num_harts_; ++hart_id)
        {
            threads_[hart_id]->getSimState()->cycles = current_cycle;
        }

        if (timed_pause)
        {
            DLOG("Starting pause counter for hart " << std::dec << current_hart_id_);
            threads_running_.reset(current_hart_id_);
            if (sim_state->sim_pause_reason == SimPauseReason::PAUSE)
            {
                ev_pause_counter_expires_.preparePayload(current_hart_id_)
                    ->schedule(current_cycle - getClock()->currentCycle()
                               + pause_counter_duration_);
            }
            else // SimPauseReason::WRS_STO
            {
                ev_wrssto_counter_expires_.preparePayload(current_hart_id_)
                    ->schedule(current_cycle - getClock()->currentCycle()
                               + wrssto_counter_duration_);
            }
        }

        // Simple round robin
        ++current_hart_id_;
        current_hart_id_ = (current_hart_id_ == num_harts_) ? 0 : current_hart_id_;

        if (threads_running_.any())
        {
            // Keep going!
            ev_advance_sim_.schedule(current_cycle - getClock()->currentCycle());
        }
    }

    // This event will be scheduled if a thread executes an instruction
    // that pauses it. Once the pause counter expires, this event will
    // unpause the thread and reschedule the advance sim event.
    void PegasusCore::pauseCounterExpires_(const HartId & hart_id)
    {
        PegasusState* state = threads_[hart_id];
        const auto pause_reason = state->getSimState()->sim_pause_reason;
        if (pause_reason == SimPauseReason::PAUSE || pause_reason == SimPauseReason::WRS_STO)
        {
            DLOG("Pause counter expired for hart" << std::dec << hart_id);
            state->unpauseHart();
            threads_running_.set(hart_id);
            if (ev_advance_sim_.isScheduled() == false)
            {
                // Update current cycle for all threads
                const uint64_t current_cycle = getClock()->currentCycle();
                for (HartId hart_id = 0; hart_id < num_harts_; ++hart_id)
                {
                    threads_[hart_id]->getSimState()->cycles = current_cycle;
                }

                // Keep going!
                ev_advance_sim_.schedule();
            }
        }
    }

    // Besides thread rescheduling and unpause, WRS.STO wakening by counter
    // will also cancel the memory write notification on reservation set.
    void PegasusCore::wrsstoCounterExpires_(const HartId & hart_id)
    {
        pauseCounterExpires_(hart_id);
        PegasusState* state = threads_[hart_id];
        state->unregisterWaitOnReservationSet();
    }

    template <bool IS_UNIT_TEST> bool PegasusCore::compare(const PegasusCore* core) const
    {
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(xlen_, core->xlen_);
        }
        else if (xlen_ != core->xlen_)
        {
            return false;
        }

        auto compression_enabled = isCompressionEnabled();
        auto other_compression_enabled = core->isCompressionEnabled();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(compression_enabled, other_compression_enabled);
        }
        else if (compression_enabled != other_compression_enabled)
        {
            return false;
        }

        auto has_hypervisor = hasHypervisor();
        auto other_has_hypervisor = core->hasHypervisor();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(has_hypervisor, other_has_hypervisor);
        }
        else if (has_hypervisor != other_has_hypervisor)
        {
            return false;
        }

        auto pc_alignment = getPcAlignment();
        auto other_pc_alignment = core->getPcAlignment();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(pc_alignment, other_pc_alignment);
        }
        else if (pc_alignment != other_pc_alignment)
        {
            return false;
        }

        auto pc_alignment_mask = getPcAlignmentMask();
        auto other_pc_alignment_mask = core->getPcAlignmentMask();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(pc_alignment_mask, other_pc_alignment_mask);
        }
        else if (pc_alignment_mask != other_pc_alignment_mask)
        {
            return false;
        }

        auto misa_ext_field_value =
            xlen_ == 32 ? getMisaExtFieldValue<uint32_t>() : getMisaExtFieldValue<uint64_t>();

        auto other_misa_ext_field_value = core->getXlen() == 32
                                              ? core->getMisaExtFieldValue<uint32_t>()
                                              : core->getMisaExtFieldValue<uint64_t>();

        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(misa_ext_field_value, other_misa_ext_field_value);
        }
        else if (misa_ext_field_value != other_misa_ext_field_value)
        {
            return false;
        }

        return true;
    }

    template bool PegasusCore::compare<false>(const PegasusCore* core) const;
    template bool PegasusCore::compare<true>(const PegasusCore* core) const;
} // namespace pegasus
