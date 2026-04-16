#include "core/PegasusCore.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "core/VecElements.hpp"
#include "core/Fetch.hpp"
#include "core/Execute.hpp"
#include "core/translate/Translate.hpp"
#include "core/Exception.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"
#include "system/PegasusSystem.hpp"
#include "system/SystemCallEmulator.hpp"
#include "core/observers/SimController.hpp"
#include "core/observers/InstructionLogger.hpp"
#include "core/observers/STFLogger.hpp"
#include "core/observers/STFValidator.hpp"
#include "inst_handlers/zicsrind/Rvzicsrind.hpp"

#include "mavis/Mavis.h"

#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "sparta/utils/SpartaTester.hpp"

#include <algorithm>

namespace pegasus
{
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

    uint64_t getInstLimit(sparta::TreeNode* rtn, uint64_t ilimit)
    {
        auto extension = sparta::notNull(rtn->createExtension("sim"));
        const uint64_t sim_ilimit =
            extension->getParameters()->getParameter("inst_limit")->getValueAs<uint64_t>();
        // Hart ilimit overrides the sim ilimit
        return (ilimit == 0) ? sim_ilimit : ilimit;
    }

    PrivMode getPrivilegeMode(const char priv)
    {
        if (priv == 'm')
        {
            return PrivMode::MACHINE;
        }
        else if (priv == 's')
        {
            return PrivMode::SUPERVISOR;
        }
        else if (priv == 'u')
        {
            return PrivMode::USER;
        }
        else
        {
            sparta_assert(false, "Unsupported privilege mode: " << priv);
        }
    }

    PegasusState::PegasusState(sparta::TreeNode* hart_tn, const PegasusStateParameters* p) :
        sparta::Unit(hart_tn),
        hart_id_(p->hart_id),
        vlen_(p->vlen),
        reg_json_file_path_(p->reg_json_file_path),
        ilimit_(getInstLimit(hart_tn->getRoot(), p->ilimit)),
        quantum_(p->quantum),
        stop_sim_on_wfi_(p->stop_sim_on_wfi),
        ulimit_stack_size_(p->ulimit_stack_size),
        stf_filename_(p->stf_filename),
        validation_stf_filename_(p->validate_with_stf),
        validate_trace_begin_(p->validate_trace_begin),
        validate_inst_begin_(p->validate_inst_begin),
        validate_fail_on_first_diff_(p->validate_fail_on_first_diff),
        priv_mode_(getPrivilegeMode(p->priv_mode)),
        isa_string_(hart_tn->getParent()
                        ->getChildAs<sparta::ParameterSet>("params")
                        ->getParameterValueAs<std::string>("isa")),
        xlen_(getXlenFromIsaString(isa_string_)),
        isa_file_path_(hart_tn->getParent()
                           ->getChildAs<sparta::ParameterSet>("params")
                           ->getParameterValueAs<std::string>("isa_file_path")),
        arch_name_(hart_tn->getParent()
                       ->getChildAs<sparta::ParameterSet>("params")
                       ->getParameterValueAs<std::string>("arch")),
        uarch_file_path_(hart_tn->getParent()
                             ->getChildAs<sparta::ParameterSet>("params")
                             ->getParameterValueAs<std::string>("uarch_file_path")),
        extension_manager_(mavis::extension_manager::riscv::RISCVExtensionManager::fromISA(
            isa_string_, isa_file_path_ + std::string("/riscv_isa_spec.json"), isa_file_path_)),
        hypervisor_enabled_(extension_manager_.isEnabled("h")),
        zicntr_enabled_(extension_manager_.isEnabled("zicntr")),
        inst_logger_(hart_tn, "inst", "Pegasus Instruction Logger"),
        stf_valid_logger_(hart_tn, "stf_valid", "Pegasus STF Validator Logger"),
        finish_action_group_("finish_inst"),
        stop_sim_action_group_("stop_sim"),
        pause_sim_action_group_("pause_sim")
    {
        const std::string profile = hart_tn->getParent()
                                        ->getChildAs<sparta::ParameterSet>("params")
                                        ->getParameterValueAs<std::string>("profile");
        if (profile.empty() == false)
        {
            extension_manager_.setProfile(profile);
        }

        const std::string init_isa = p->isa;
        if (init_isa.empty() == false)
        {
            isa_string_ = p->isa;
            extension_manager_.setISA(isa_string_);
        }

        setPcAlignment_();

        // Set up register sets
        const std::string reg_json_file_path =
            reg_json_file_path_ + "/rv" + std::to_string(xlen_) + "/gen";
        int_rset_ = RegisterSet::create(hart_tn, reg_json_file_path + std::string("/reg_int.json"),
                                        "int_regs");
        fp_rset_ = RegisterSet::create(hart_tn, reg_json_file_path + std::string("/reg_fp.json"),
                                       "fp_regs");
        const std::string vec_reg_json = "/reg_vec" + std::to_string(vlen_) + ".json";
        vec_rset_ = RegisterSet::create(hart_tn, reg_json_file_path + vec_reg_json, "vec_regs");
        csr_rset_ = RegisterSet::create(
            hart_tn, reg_json_file_path + std::string("/reg_csr_hart.json"), "csr_regs");

        auto add_registers = [this](const auto & reg_set)
        {
            for (const auto & kvp : reg_set->getRegistersByName())
            {
                registers_by_name_[kvp.first] = kvp.second;
                for (auto & alias_name : kvp.second->getAliases())
                {
                    registers_by_name_[alias_name] = kvp.second;
                }
            }
        };

        registers_by_name_.reserve(int_rset_->getNumRegisters() + fp_rset_->getNumRegisters()
                                   + vec_rset_->getNumRegisters() + csr_rset_->getNumRegisters());
        add_registers(int_rset_);
        add_registers(fp_rset_);
        add_registers(vec_rset_);
        add_registers(csr_rset_);

        // Add CSR callbacks and increment PC Action
        const bool CHECK_ILIMIT = ilimit_ > 0;
        if (xlen_ == 32)
        {
            addCSRRegisterCallbacks_<RV32>();

            if (CHECK_ILIMIT)
            {
                increment_pc_action_ =
                    pegasus::Action::createAction<&PegasusState::incrementPc_<RV32, true>>(
                        this, "increment pc");
            }
            else
            {
                increment_pc_action_ =
                    pegasus::Action::createAction<&PegasusState::incrementPc_<RV32, false>>(
                        this, "increment pc");
            }
        }
        else if (xlen_ == 64)
        {
            addCSRRegisterCallbacks_<RV64>();

            if (CHECK_ILIMIT)
            {
                increment_pc_action_ =
                    pegasus::Action::createAction<&PegasusState::incrementPc_<RV64, true>>(
                        this, "increment pc");
            }
            else
            {
                increment_pc_action_ =
                    pegasus::Action::createAction<&PegasusState::incrementPc_<RV64, false>>(
                        this, "increment pc");
            }
        }
        else
        {
            sparta_assert(false, "Unsupported XLEN");
        }

        // Add increment PC Action to finish ActionGroup
        finish_action_group_.addAction(increment_pc_action_);

        // Create Action to stop simulation
        stop_action_ = pegasus::Action::createAction<&PegasusState::stopSim_>(this, "stop sim");
        stop_action_.addTag(ActionTags::STOP_SIM_TAG);
        stop_sim_action_group_.addAction(stop_action_);

        // Create Action to pause simulation
        pause_action_ = pegasus::Action::createAction<&PegasusState::pauseSim_>(this, "pause sim");
        pause_sim_action_group_.addAction(pause_action_);

        // Update VectorConfig vlen
        vector_config_.setVLEN(vlen_);
    }

    mavis::FileNameListType PegasusState::getUArchFiles_() const
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

    // Not default -- defined in source file to reduce massive inlining
    PegasusState::~PegasusState() {}

    uint64_t PegasusState::getXlen() const { return xlen_; }

    void PegasusState::onBindTreeEarly_()
    {
        auto hart_tn = getContainer();

        fetch_unit_ = hart_tn->getChild("fetch")->getResourceAs<Fetch*>();
        execute_unit_ = hart_tn->getChild("execute")->getResourceAs<Execute*>();
        translate_unit_ = hart_tn->getChild("translate")->getResourceAs<Translate*>();
        exception_unit_ = hart_tn->getChild("exception")->getResourceAs<Exception*>();

        // Connect finish ActionGroup to Fetch
        finish_action_group_.setNextActionGroup(fetch_unit_->getActionGroup());

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

        updateCsrEnabledState_();
    }

    void PegasusState::onBindTreeLate_()
    {
        // Make sure privilege mode at boot is supported
        sparta_assert(pegasus_core_->isPrivilegeModeSupported(priv_mode_),
                      "Attempting to change privilege mode to an unsupported mode: " << priv_mode_);

        // Validate the ISA string
        for (const auto & ext : extension_manager_.getEnabledExtensions(false))
        {
            if (false == pegasus_core_->isExtensionSupported(xlen_, ext.first))
            {
                sparta_assert(false, "ISA extension: " << ext.first
                                                       << " is not supported in isa_string: "
                                                       << isa_string_);
            }
        }

        // Set up translation
        if (xlen_ == 64)
        {
            updateTranslationMode<RV64>(translate_types::TranslationStage::SUPERVISOR);
            if (hasHypervisor())
            {
                updateTranslationMode<RV64>(translate_types::TranslationStage::VIRTUAL_SUPERVISOR);
                updateTranslationMode<RV64>(translate_types::TranslationStage::GUEST);
            }
        }
        else
        {
            updateTranslationMode<RV32>(translate_types::TranslationStage::SUPERVISOR);
            if (hasHypervisor())
            {
                updateTranslationMode<RV32>(translate_types::TranslationStage::VIRTUAL_SUPERVISOR);
                updateTranslationMode<RV32>(translate_types::TranslationStage::GUEST);
            }
        }

        // Update vlenb csr
        if (xlen_ == 32)
        {
            csr_rset_->getRegister(VLENB)->dmiWrite<uint32_t>(vlen_ / 8);
        }
        else // 64
        {
            csr_rset_->getRegister(VLENB)->dmiWrite<uint64_t>(vlen_ / 8);
        }

        /* Only after state has been fully initialized can we start creating Observers. */

        // FIXME: Does Sparta have a callback notif for when debug icount is reached?
        if (inst_logger_.observed())
        {
            if (xlen_ == 64)
            {
                addObserver(std::make_unique<InstructionLogger>(inst_logger_, ObserverMode::RV64));
            }
            else
            {
                addObserver(std::make_unique<InstructionLogger>(inst_logger_, ObserverMode::RV32));
            }
        }

        if (!stf_filename_.empty())
        {
            addObserver(std::make_unique<STFLogger>(xlen_, pc_, stf_filename_, this));
        }

        if (!validation_stf_filename_.empty())
        {
            if (xlen_ == 64)
            {
                addObserver(std::make_unique<STFValidator>(
                    stf_valid_logger_, ObserverMode::RV64, validation_stf_filename_,
                    validate_trace_begin_, validate_inst_begin_, validate_fail_on_first_diff_));
            }
            else
            {
                addObserver(std::make_unique<STFValidator>(
                    stf_valid_logger_, ObserverMode::RV32, validation_stf_filename_,
                    validate_trace_begin_, validate_inst_begin_, validate_fail_on_first_diff_));
            }
        }
    }

    void PegasusState::setPrivMode(PrivMode priv_mode, bool virt_mode)
    {
        sparta_assert(pegasus_core_->isPrivilegeModeSupported(priv_mode),
                      "Attempting to change privilege mode to an unsupported mode: " << priv_mode);
        virtual_mode_ = virt_mode && (priv_mode != PrivMode::MACHINE);
        priv_mode_ = priv_mode;
    }

    template <typename XLEN>
    void PegasusState::updateTranslationMode(const translate_types::TranslationStage stage)
    {
        static const std::vector<translate_types::TranslationMode> mmu_mode_map = {
            translate_types::TranslationMode::BAREMETAL, // mode == 0
            translate_types::TranslationMode::SV32,      // mode == 1 xlen==32
            translate_types::TranslationMode::INVALID,   // mode == 2 - 7 -> reserved
            translate_types::TranslationMode::INVALID,   translate_types::TranslationMode::INVALID,
            translate_types::TranslationMode::INVALID,   translate_types::TranslationMode::INVALID,
            translate_types::TranslationMode::INVALID,
            translate_types::TranslationMode::SV39, // mode ==  8, xlen==64
            translate_types::TranslationMode::SV48, // mode ==  9, xlen==64
            translate_types::TranslationMode::SV57  // mode == 10, xlen==64
        };

        const uint32_t ATP_CSR = Translate::getAtpCsr(stage);
        const uint32_t atp_mode_val = READ_CSR_FIELD<XLEN>(this, ATP_CSR, "mode");
        sparta_assert(atp_mode_val < mmu_mode_map.size(), "atp mode: " << atp_mode_val);
        const translate_types::TranslationMode mode = mmu_mode_map[atp_mode_val];

        // FIXME: Hypervisor does not support MPRV yet
        const uint32_t mprv_val = READ_CSR_FIELD<XLEN>(this, MSTATUS, "mprv");
        const PrivMode prev_priv_mode = (PrivMode)READ_CSR_FIELD<XLEN>(this, MSTATUS, "mpp");
        ldst_priv_modes_.at(static_cast<uint32_t>(stage)) =
            (mprv_val == 1) ? prev_priv_mode : priv_mode_;
        const translate_types::TranslationMode ls_mode =
            (ldst_priv_modes_.at(static_cast<uint32_t>(stage)) == PrivMode::MACHINE)
                ? translate_types::TranslationMode::BAREMETAL
                : mode;

        DLOG_CODE_BLOCK(DLOG_OUTPUT(stage << " MMU Mode: " << mode);
                        DLOG_OUTPUT(stage << " MMU LS Mode: " << ls_mode););
        switch (stage)
        {
            case translate_types::TranslationStage::SUPERVISOR:
                translate_unit_
                    ->updateTranslationMode<XLEN, translate_types::TranslationStage::SUPERVISOR>(
                        mode, ls_mode);
                break;
            case translate_types::TranslationStage::VIRTUAL_SUPERVISOR:
                translate_unit_->updateTranslationMode<
                    XLEN, translate_types::TranslationStage::VIRTUAL_SUPERVISOR>(mode, ls_mode);
                break;
            case translate_types::TranslationStage::GUEST:
                translate_unit_
                    ->updateTranslationMode<XLEN, translate_types::TranslationStage::GUEST>(
                        mode, ls_mode);
                break;
            case translate_types::TranslationStage::INVALID:
                sparta_assert(false, "Translation stage cannot be INVALID!");
        }
    }

    void PegasusState::pauseHart(const SimPauseReason reason)
    {
        sim_state_.sim_pause_reason = reason;
        finish_action_group_.setNextActionGroup(&pause_sim_action_group_);
    }

    void PegasusState::unpauseHart()
    {
        sim_state_.sim_pause_reason = SimPauseReason::INVALID;
        // We replace the next ActionGroup pointer to pause the sim, so it needs to
        // be set back to Fetch
        finish_action_group_.setNextActionGroup(fetch_unit_->getActionGroup());
    }

    sparta::Register* PegasusState::getSpartaRegister(const mavis::OperandInfo::Element* operand)
    {
        if (operand)
        {
            switch (operand->operand_type)
            {
                case mavis::InstMetaData::OperandTypes::WORD:
                case mavis::InstMetaData::OperandTypes::LONG:
                    return getIntRegister(operand->field_value);
                case mavis::InstMetaData::OperandTypes::HALF:
                case mavis::InstMetaData::OperandTypes::SINGLE:
                case mavis::InstMetaData::OperandTypes::DOUBLE:
                case mavis::InstMetaData::OperandTypes::QUAD:
                    return getFpRegister(operand->field_value);
                case mavis::InstMetaData::OperandTypes::VECTOR:
                    return getVecRegister(operand->field_value);
                case mavis::InstMetaData::OperandTypes::NONE:
                    sparta_assert(false, "Invalid Mavis Operand Type!");
            }
        }

        return nullptr;
    }

    Action::ItrType PegasusState::preExecute_(PegasusState* state, Action::ItrType action_it)
    {
        for (const auto & observer : observers_)
        {
            observer->preExecute(state);
        }

        return ++action_it;
    }

    Action::ItrType PegasusState::postExecute_(PegasusState* state, Action::ItrType action_it)
    {
        for (const auto & observer : observers_)
        {
            observer->postExecute(state);
        }

        return ++action_it;
    }

    Action::ItrType PegasusState::preException_(PegasusState* state, Action::ItrType action_it)
    {
        for (const auto & observer : observers_)
        {
            observer->preException(state);
        }

        return ++action_it;
    }

    void PegasusState::changeMavisContext()
    {
        extension_manager_.switchMavisContext(*mavis_.get());

        hypervisor_enabled_ = extension_manager_.isEnabled("h");
        zicntr_enabled_ = extension_manager_.isEnabled("zicntr");

        setPcAlignment_();

        updateCsrEnabledState_();
    }

    template <typename XLEN> uint32_t PegasusState::getMisaExtFieldValue() const
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

    template uint32_t PegasusState::getMisaExtFieldValue<RV32>() const;
    template uint32_t PegasusState::getMisaExtFieldValue<RV64>() const;

    void PegasusState::enableInteractiveMode()
    {
        sparta_assert(sim_controller_ == nullptr, "Interactive mode is already enabled");
        auto observer = std::make_unique<SimController>();
        sim_controller_ = observer.get();
        addObserver(std::move(observer));
    }

    void PegasusState::useSpikeFormatting()
    {
        for (auto & obs : observers_)
        {
            if (auto inst_logger = dynamic_cast<InstructionLogger*>(obs.get()))
            {
                inst_logger->useSpikeFormatting();
            }
        }
    }

    sparta::Register* PegasusState::findRegister(const std::string & reg_name,
                                                 bool must_exist) const
    {
        auto iter = registers_by_name_.find(reg_name);
        auto reg = (iter != registers_by_name_.end()) ? iter->second : nullptr;
        sparta_assert(!must_exist || reg, "Failed to find register: " << reg_name);
        return reg;
    }

    template <typename MemoryType>
    bool PegasusState::readMemory(const PegasusTranslationState::TranslationResult & result,
                                  std::vector<uint8_t> & buffer, const MemAccessSource source)
    {
        auto* memory = pegasus_core_->getMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        buffer.resize(sizeof(MemoryType) / sizeof(uint8_t));
        const MemorySupplement supplement{result.getPAddr(), result.getVAddr(), source};
        const bool success = memory->tryRead(result.getPAddr(), size, buffer.data(), &supplement);
        DLOG("Memory read (" << source << ", " << std::dec << size << "B) to 0x" << std::hex
                             << result.getPAddr() << " " << (success ? "succeeded!" : "failed!"));
        return success;
    }

    template <typename MemoryType>
    bool PegasusState::readMemory(const Addr paddr, std::vector<uint8_t> & buffer,
                                  const MemAccessSource source)
    {
        const Addr vaddr = 0;
        const PegasusTranslationState::TranslationResult result{vaddr, paddr, sizeof(MemoryType)};
        return readMemory<MemoryType>(result, buffer, source);
    }

    template <typename MemoryType>
    bool PegasusState::writeMemory(const PegasusTranslationState::TranslationResult & result,
                                   const MemoryType value, const MemAccessSource source)
    {
        auto* memory = pegasus_core_->getMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        const std::vector<uint8_t> buffer = convertToByteVector<MemoryType>(value);
        const MemorySupplement supplement{result.getPAddr(), result.getVAddr(), source};
        const bool success = memory->tryWrite(result.getPAddr(), size, buffer.data(), &supplement);
        DLOG("Memory write (" << source << ", " << std::dec << size << "B) to 0x" << std::hex
                              << result.getPAddr() << " (value: 0x" << (uint64_t)value << ") "
                              << (success ? "succeeded!" : "failed!"));
        return success;
    }

    template <typename MemoryType>
    bool PegasusState::writeMemory(const Addr paddr, const MemoryType value,
                                   const MemAccessSource source)
    {
        const Addr vaddr = 0;
        const PegasusTranslationState::TranslationResult result{vaddr, paddr, sizeof(MemoryType)};
        return writeMemory<MemoryType>(result, value, source);
    }

#define INSTANTIATE_READ_MEMORY_METHODS(SIZE)                                                      \
    template bool PegasusState::readMemory<SIZE>(                                                  \
        const PegasusTranslationState::TranslationResult &, std::vector<uint8_t> &,                \
        const MemAccessSource);                                                                    \
    template bool PegasusState::readMemory<SIZE>(const Addr, std::vector<uint8_t> &,               \
                                                 const MemAccessSource);

    INSTANTIATE_READ_MEMORY_METHODS(int8_t)
    INSTANTIATE_READ_MEMORY_METHODS(uint8_t)
    INSTANTIATE_READ_MEMORY_METHODS(int16_t)
    INSTANTIATE_READ_MEMORY_METHODS(uint16_t)
    INSTANTIATE_READ_MEMORY_METHODS(int32_t)
    INSTANTIATE_READ_MEMORY_METHODS(uint32_t)
    INSTANTIATE_READ_MEMORY_METHODS(int64_t)
    INSTANTIATE_READ_MEMORY_METHODS(uint64_t)

#define INSTANTIATE_WRITE_MEMORY_METHODS(SIZE)                                                     \
    template bool PegasusState::writeMemory<SIZE>(                                                 \
        const PegasusTranslationState::TranslationResult &, const SIZE, const MemAccessSource);    \
    template bool PegasusState::writeMemory<SIZE>(const Addr, const SIZE, const MemAccessSource);

    INSTANTIATE_WRITE_MEMORY_METHODS(uint8_t)
    INSTANTIATE_WRITE_MEMORY_METHODS(uint16_t)
    INSTANTIATE_WRITE_MEMORY_METHODS(uint32_t)
    INSTANTIATE_WRITE_MEMORY_METHODS(uint64_t)

    void PegasusState::addObserver(std::unique_ptr<Observer> observer)
    {
        if (observers_.empty())
        {
            pre_execute_action_ =
                pegasus::Action::createAction<&PegasusState::preExecute_>(this, "pre execute");
            post_execute_action_ =
                pegasus::Action::createAction<&PegasusState::postExecute_>(this, "post execute");
            pre_exception_action_ =
                pegasus::Action::createAction<&PegasusState::preException_>(this, "pre exception");

            finish_action_group_.addAction(post_execute_action_);
            exception_unit_->getActionGroup()->insertActionBefore(pre_exception_action_,
                                                                  ActionTags::EXCEPTION_TAG);
        }

        pegasus_core_->getSystem()->registerMemoryCallbacks(observer.get());
        for (auto reg : csr_rset_->getRegisters())
        {
            observer->registerReadWriteCsrCallbacks(reg);
        }

        observers_.emplace_back(std::move(observer));
    }

    void PegasusState::insertExecuteActions(ActionGroup* action_group, const bool is_memory_inst)
    {
        if (pre_execute_action_)
        {
            if (is_memory_inst)
            {
                action_group->insertActionBefore(pre_execute_action_, ActionTags::COMPUTE_ADDR_TAG);
            }
            else
            {
                action_group->insertActionBefore(pre_execute_action_, ActionTags::EXECUTE_TAG);
            }
        }
    }

    template <typename XLEN> void PegasusState::incrCycleCsrs_()
    {
        const XLEN cycle = getCsrRegister(CYCLE)->dmiRead<XLEN>();
        const XLEN mcycle = getCsrRegister(MCYCLE)->dmiRead<XLEN>();
        if constexpr (std::is_same_v<XLEN, RV32>)
        {
            if (SPARTA_EXPECT_FALSE(cycle == std::numeric_limits<XLEN>::max()))
            {
                const XLEN cycleh = getCsrRegister(CYCLE)->dmiRead<XLEN>();
                getCsrRegister(CYCLEH)->dmiWrite<XLEN>(cycleh + 1);
            }

            if (SPARTA_EXPECT_FALSE(mcycle == std::numeric_limits<XLEN>::max()))
            {
                const XLEN mcycleh = getCsrRegister(MCYCLE)->dmiRead<XLEN>();
                getCsrRegister(MCYCLEH)->dmiWrite<XLEN>(mcycleh + 1);
            }
        }
        getCsrRegister(CYCLE)->dmiWrite<XLEN>(cycle + 1);
        getCsrRegister(MCYCLE)->dmiWrite<XLEN>(mcycle + 1);
    }

    template <typename XLEN> void PegasusState::incrTimeCsrs_()
    {
        // From the RISC-V spec:
        // On some simple platforms, cycle count might represent a valid
        // implementation of RDTIME, in which case RDTIME and RDCYCLE
        // may return the same result.
        const XLEN time = getCsrRegister(TIME)->dmiRead<XLEN>();
        if constexpr (std::is_same_v<XLEN, RV32>)
        {
            if (SPARTA_EXPECT_FALSE(time == std::numeric_limits<XLEN>::max()))
            {
                const XLEN timeh = getCsrRegister(TIME)->dmiRead<XLEN>();
                getCsrRegister(TIMEH)->dmiWrite<XLEN>(timeh + 1);
            }
        }
        getCsrRegister(TIME)->dmiWrite<XLEN>(time + 1);
    }

    template <typename XLEN> void PegasusState::incrInstretCsrs_()
    {
        const XLEN instret = getCsrRegister(INSTRET)->dmiRead<XLEN>();
        const XLEN minstret = getCsrRegister(MINSTRET)->dmiRead<XLEN>();
        if constexpr (std::is_same_v<XLEN, RV32>)
        {
            if (SPARTA_EXPECT_FALSE(instret == std::numeric_limits<XLEN>::max()))
            {
                const XLEN instreth = getCsrRegister(INSTRETH)->dmiRead<XLEN>();
                getCsrRegister(INSTRETH)->dmiWrite<XLEN>(instreth + 1);
            }

            if (SPARTA_EXPECT_FALSE(minstret == std::numeric_limits<XLEN>::max()))
            {
                const XLEN minstreth = getCsrRegister(MINSTRETH)->dmiRead<XLEN>();
                getCsrRegister(MINSTRETH)->dmiWrite<XLEN>(minstreth + 1);
            }
        }
        getCsrRegister(INSTRET)->dmiWrite<XLEN>(instret + 1);
        getCsrRegister(MINSTRET)->dmiWrite<XLEN>(minstret + 1);
    }

    template void PegasusState::incrCycleCsrs_<RV64>();
    template void PegasusState::incrTimeCsrs_<RV64>();
    template void PegasusState::incrInstretCsrs_<RV64>();
    template void PegasusState::incrCycleCsrs_<RV32>();
    template void PegasusState::incrTimeCsrs_<RV32>();
    template void PegasusState::incrInstretCsrs_<RV32>();

    template <typename XLEN, bool CHECK_ILIMIT>
    Action::ItrType PegasusState::incrementPc_(PegasusState*, Action::ItrType action_it)
    {
        // Clear inst translation state
        inst_translation_state_.reset();

        // Set PC
        prev_pc_ = pc_;
        pc_ = next_pc_;
        DLOG("PC: 0x" << std::hex << pc_);

        // Increment instruction count
        ++sim_state_.inst_count;

        // TODO: We don't have a timing model yet so
        // for now just assume each inst takes 1 cycle
        ++sim_state_.cycles;

        if (hasZicntr())
        {
            incrInstretCsrs_<XLEN>();
            incrCycleCsrs_<XLEN>();
            incrTimeCsrs_<XLEN>();
        }

        if constexpr (CHECK_ILIMIT)
        {
            if (sim_state_.inst_count == ilimit_)
            {
                std::cout << "Reached instruction limit (" << std::dec << ilimit_
                          << "), stopping simulation." << std::endl;
                const uint64_t exit_code = 0;
                setSimStopped(true, exit_code);
            }
        }

        if (sim_state_.inst_count % quantum_ == 0)
        {
            DLOG("Executed " << std::dec << quantum_
                             << " instructions (total: " << sim_state_.inst_count << ")");
            pauseHart(SimPauseReason::QUANTUM);
        }

        return ++action_it;
    }

    template <bool IS_UNIT_TEST> bool PegasusState::compare(const PegasusState* state) const
    {
        auto pc = getPc();
        auto other_pc = state->getPc();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(pc, other_pc);
        }
        else if (pc != other_pc)
        {
            return false;
        }

        auto priv_mode = getPrivMode();
        auto other_priv_mode = state->getPrivMode();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(priv_mode, other_priv_mode);
        }
        else if (priv_mode != other_priv_mode)
        {
            return false;
        }

        auto ldst_priv_mode = getLdstPrivMode();
        auto other_ldst_priv_mode = state->getLdstPrivMode();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(ldst_priv_mode, other_ldst_priv_mode);
        }
        else if (ldst_priv_mode != other_ldst_priv_mode)
        {
            return false;
        }

        auto virtual_mode = getVirtualMode();
        auto other_virtual_mode = state->getVirtualMode();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(virtual_mode, other_virtual_mode);
        }
        else if (virtual_mode != other_virtual_mode)
        {
            return false;
        }

        auto curr_excp = getCurrentException();
        auto other_curr_excp = state->getCurrentException();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(curr_excp, other_curr_excp);
        }
        else if (curr_excp != other_curr_excp)
        {
            return false;
        }

        auto other_xlen = state->getXlen();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(xlen_, other_xlen);
        }
        else if (xlen_ != other_xlen)
        {
            return false;
        }

        auto has_hypervisor = hasHypervisor();
        auto other_has_hypervisor = state->hasHypervisor();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(has_hypervisor, other_has_hypervisor);
        }
        else if (has_hypervisor != other_has_hypervisor)
        {
            return false;
        }

        auto has_zicntr = hasZicntr();
        auto other_has_zicntr = state->hasZicntr();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(has_zicntr, other_has_zicntr);
        }
        else if (has_zicntr != other_has_zicntr)
        {
            return false;
        }

        auto misa_ext_field_value =
            xlen_ == 32 ? getMisaExtFieldValue<uint32_t>() : getMisaExtFieldValue<uint64_t>();
        auto other_misa_ext_field_value = getXlen() == 32 ? state->getMisaExtFieldValue<uint32_t>()
                                                          : state->getMisaExtFieldValue<uint64_t>();
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

    template <bool IS_UNIT_TEST>
    bool PegasusState::SimState::compare(const SimState* sim_state) const
    {
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(current_opcode, sim_state->current_opcode);
        }
        else if (current_opcode != sim_state->current_opcode)
        {
            return false;
        }

        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(current_uid, sim_state->current_uid);
        }
        else if (current_uid != sim_state->current_uid)
        {
            return false;
        }

        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(inst_count, sim_state->inst_count);
        }
        else if (inst_count != sim_state->inst_count)
        {
            return false;
        }

        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(sim_stopped, sim_state->sim_stopped);
        }
        else if (sim_stopped != sim_state->sim_stopped)
        {
            return false;
        }

        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(test_passed, sim_state->test_passed);
        }
        else if (test_passed != sim_state->test_passed)
        {
            return false;
        }

        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(workload_exit_code, sim_state->workload_exit_code);
        }
        else if (workload_exit_code != sim_state->workload_exit_code)
        {
            return false;
        }

        return true;
    }

    void PegasusState::setSimStopped(bool sim_stopped, const int64_t exit_code)
    {
        sim_state_.sim_stopped = sim_stopped;
        if (sim_stopped)
        {
            std::cout << "Stopping hart" << std::dec << hart_id_ << std::endl;

            if (hasZicntr())
            {
                if (xlen_ == 64)
                {
                    std::cout << "\tCYCLE: " << getCsrRegister(CYCLE)->dmiRead<RV64>() << std::endl;
                    std::cout << "\tTIME: " << getCsrRegister(TIME)->dmiRead<RV64>() << std::endl;
                    std::cout << "\tINSTRET: " << getCsrRegister(INSTRET)->dmiRead<RV64>()
                              << std::endl;
                }
                else
                {
                    std::cout << "\tCYCLE: " << getCsrRegister(CYCLE)->dmiRead<RV32>() << std::endl;
                    std::cout << "\tTIME: " << getCsrRegister(TIME)->dmiRead<RV32>() << std::endl;
                    std::cout << "\tINSTRET: " << getCsrRegister(INSTRET)->dmiRead<RV32>()
                              << std::endl;
                }
            }

            sim_state_.workload_exit_code = exit_code;
            sim_state_.test_passed = (exit_code == 0) ? true : false;

            finish_action_group_.setNextActionGroup(&stop_sim_action_group_);
        }
        else
        {
            sparta_assert(exit_code == 0,
                          "Exit code should not be set if simulation is being resumed. Exit code: "
                              << std::dec << exit_code);
            sim_state_.workload_exit_code = 0;
            sim_state_.test_passed = true;

            finish_action_group_.setNextActionGroup(fetch_unit_->getActionGroup());
        }
    }

    template <typename XLEN> XLEN PegasusState::emulateSystemCall()
    {
        // x10 -> x16 are the function arguments.
        // x17 holds the system call number, first item on the stack
        SystemCallStack call_stack = {READ_INT_REG<XLEN>(this, 17), READ_INT_REG<XLEN>(this, 10),
                                      READ_INT_REG<XLEN>(this, 11), READ_INT_REG<XLEN>(this, 12),
                                      READ_INT_REG<XLEN>(this, 13), READ_INT_REG<XLEN>(this, 14),
                                      READ_INT_REG<XLEN>(this, 15), READ_INT_REG<XLEN>(this, 16)};

        auto mem = getCore()->getMemory();
        auto emulator = getCore()->getSystemCallEmulator();
        const XLEN ret_code = static_cast<XLEN>(emulator->emulateSystemCall(call_stack, mem));
        return ret_code;
    }

    template RV64 PegasusState::emulateSystemCall<RV64>();
    template RV32 PegasusState::emulateSystemCall<RV32>();

    // Initialize a program stack (argc, argv, envp, auxv, etc)
    // Useful info about ELF binaries: https://lwn.net/Articles/631631/
    // This is used mostly for system call emulation
    void PegasusState::setupProgramStack(const std::vector<std::string> & program_arguments)
    {
        if (false == pegasus_core_->isSystemCallEmulationEnabled())
        {
            // System call emulation is not enabled.  There's a good
            // chance we might be running a bare metal binary so no
            // need to set up prog arguments.  In any event, we better
            // not receive any either.
            sparta_assert(
                program_arguments.size() == 1,
                "System Call emulation is not enabled, but the program is given arguments: "
                    << program_arguments);
            return;
        }

        //
        // Taken from this awesome article:
        //
        // http://articles.manugarg.com/aboutelfauxiliaryvectors.html
        //
        // position          content                       size (bytes) + comment
        //------------------------------------------------------------------------
        // stack pointer ->  [ argc = number of args ]     4
        //                   [ argv[0] (pointer) ]         4   (program name)
        //                   [ argv[1] (pointer) ]         4
        //                   [ argv[..] (pointer) ]        4 * x
        //                   [ argv[n - 1] (pointer) ]     4
        //                   [ argv[n] (pointer) ]         4   (= NULL)
        //                   [ envp[0] (pointer) ]         4
        //                   [ envp[1] (pointer) ]         4
        //                   [ envp[..] (pointer) ]        4
        //                   [ envp[term] (pointer) ]      4   (= NULL)
        //                   [ auxv[0] (Elf32_auxv_t) ]    8
        //                   [ auxv[1] (Elf32_auxv_t) ]    8
        //                   [ auxv[..] (Elf32_auxv_t) ]   8
        //                   [ auxv[term] (Elf32_auxv_t) ] 8   (= AT_NULL vector)
        //                   [ padding ]                   0 - 16
        //                   [ argument ASCIIZ strings ]   >= 0
        //                   [ environment ASCIIZ str. ]   >= 0
        // (0xbffffffc)      [ end marker ]                4   (= NULL)
        // (0xc0000000)      < bottom of stack >           0   (virtual)
        //
        // The argument stack contains env, followed by command line
        // arguments, auxv data, ELF aux vector info, environment,
        // argv, and then argc.  The SP starts at argc and grows
        // towards upward.

        // Get the SP (aka x2)
        const bool MUST_EXIST = true;
        sparta::Register* reg = findRegister("sp", MUST_EXIST);
        uint64_t sp = 0;

        if (xlen_ == 64)
        {
            sp = reg->dmiRead<RV64>();
            sparta_assert(std::numeric_limits<uint64_t>::max() - sp > ulimit_stack_size_,
                          "Stack pointer initial value has a good chance of overflowing");
        }
        else
        {
            sp = reg->dmiRead<RV32>();
            sparta_assert(std::numeric_limits<uint32_t>::max() - (uint32_t)sp > ulimit_stack_size_,
                          "Stack pointer initial value has a good chance of overflowing");
        }
        sparta_assert(sp != 0, "The stack pointer (sp aka x2) is set to 0.  Use --reg \"sp <val>\" "
                               "to set it to something...smarter");

        auto* memory = sparta::notNull(pegasus_core_->getSystem())->getSystemMemory();
        sparta_assert(memory != nullptr, "Got no memory to preload with the argument stack");

        ////////////////////////////////////////////////////////////////////////////////
        // Set up argc
        uint64_t data = program_arguments.size();
        memory->poke(sp, 8, (uint8_t*)&data);
        sp += 8;

        ////////////////////////////////////////////////////////////////////////////////
        // Set up argv pointers space -- where the arguments for the
        // program will be found in memory
        auto argv_sp_addr = sp; // Remember this address for later
        sp += 8 * program_arguments.size();
        data = 0;
        // Write (null) or end of argv arguments
        memory->poke(sp, 8, (uint8_t*)&data);
        sp += 8;

        ////////////////////////////////////////////////////////////////////////////////
        // Set up envp -- empty for now
        std::vector<std::string> env_vars;
        auto envp_sp_addr = sp;
        sp += 8 * env_vars.size();
        data = 0;
        memory->poke(sp, 8, (uint8_t*)&data); // Write (nil)
        sp += 8;

        ////////////////////////////////////////////////////////////////////////////////
        // Set up auxv
        // Magic...
        // https://github.com/pytorch/cpuinfo/blob/6c9eb84ba310f237cea13c478be50102e1128e9b/src/riscv/linux/riscv-isa.c
        const auto AT_HWCAP = 16;
        const auto COMPAT_HWCAP_ISA_I(1 << ('I' - 'A'));
        const auto COMPAT_HWCAP_ISA_M(1 << ('M' - 'A'));
        const auto COMPAT_HWCAP_ISA_A(1 << ('A' - 'A'));
        const auto COMPAT_HWCAP_ISA_F(1 << ('F' - 'A'));
        const auto COMPAT_HWCAP_ISA_D(1 << ('D' - 'A'));
        const auto COMPAT_HWCAP_ISA_C(1 << ('C' - 'A'));
        const auto COMPAT_HWCAP_ISA_V(1 << ('V' - 'A'));

        uint64_t a_val = COMPAT_HWCAP_ISA_I | COMPAT_HWCAP_ISA_M | COMPAT_HWCAP_ISA_A;
        if (isExtensionEnabled("f"))
        {
            a_val |= COMPAT_HWCAP_ISA_F;
        }
        if (isExtensionEnabled("d"))
        {
            a_val |= COMPAT_HWCAP_ISA_D;
        }
        if (isExtensionEnabled("c"))
        {
            a_val |= COMPAT_HWCAP_ISA_C;
        }
        if (isExtensionEnabled("v"))
        {
            a_val |= COMPAT_HWCAP_ISA_V;
        }
        uint64_t auxv[2] = {AT_HWCAP, a_val};
        memory->poke(sp, 16, (uint8_t*)&auxv);
        sp += 16;

        // padding
        memory->poke(sp, 8, (uint8_t*)&data); // Write (null)
        sp += 8;
        memory->poke(sp, 8, (uint8_t*)&data); // Write (null)
        sp += 8;

        ////////////////////////////////////////////////////////////////////////////////
        // argv string addresses -- need to pad to 16B boundary
        sp &= ~0xf;
        for (const auto & arg : program_arguments)
        {
            // Add 1 to include the null character
            const auto str_len = arg.size() + 1;
            ILOG("Pushing argv: " << arg);
            memory->poke(sp, str_len, (uint8_t*)arg.data());
            memory->poke(argv_sp_addr, 8, (uint8_t*)&sp);
            sp += str_len;
            argv_sp_addr += 8;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // envp string addresses -- just like argv
        sp += 16;
        sp &= ~0xf;
        for (const auto & envp : env_vars)
        {
            const auto str_len = envp.size() + 1;
            memory->poke(sp, str_len, (uint8_t*)envp.data());
            memory->poke(envp_sp_addr, 8, (uint8_t*)&sp);
            sp += str_len;
            envp_sp_addr += 8;
        }
    }

    void PegasusState::boot()
    {
        std::cout << "Booting hartid " << std::dec << hart_id_ << std::endl;
        {
            PegasusState* state = this;

            if (xlen_ == 64)
            {
                POKE_CSR_REG<RV64>(this, MHARTID, hart_id_);

                const uint64_t xlen_val = 2;
                POKE_CSR_FIELD<RV64>(this, MISA, "mxl", xlen_val);

                const uint32_t ext_val = getMisaExtFieldValue<RV64>();
                POKE_CSR_FIELD<RV64>(this, MISA, "extensions", ext_val);

                // Initialize MSTATUS/STATUS with User and Supervisor mode XLEN
                POKE_CSR_FIELD<RV64>(this, MSTATUS, "uxl", xlen_val);
                POKE_CSR_FIELD<RV64>(this, MSTATUS, "sxl", xlen_val);
                POKE_CSR_FIELD<RV64>(this, SSTATUS, "uxl", xlen_val);

                if (hasHypervisor())
                {
                    POKE_CSR_FIELD<RV64>(this, VSSTATUS, "uxl", xlen_val);
                }
            }
            else
            {
                POKE_CSR_REG<RV32>(this, MHARTID, hart_id_);

                const uint32_t xlen_val = 1;
                POKE_CSR_FIELD<RV32>(this, MISA, "mxl", xlen_val);

                const uint32_t ext_val = getMisaExtFieldValue<RV32>();
                POKE_CSR_FIELD<RV32>(this, MISA, "extensions", ext_val);
            }

            std::cout << "PegasusState::boot()\n";
            std::cout << std::hex;
            std::cout << "\tMHARTID: " << state->getCsrRegister(MHARTID)->dmiRead<uint64_t>()
                      << std::endl;
            std::cout << "\tMISA:    " << state->getCsrRegister(MISA)->dmiRead<uint64_t>()
                      << std::endl;
            std::cout << "\tMSTATUS: " << state->getCsrRegister(MSTATUS)->dmiRead<uint64_t>()
                      << std::endl;
            std::cout << "\tSSTATUS: " << state->getCsrRegister(SSTATUS)->dmiRead<uint64_t>()
                      << std::endl;
            std::cout << std::dec;
        }

        if (sim_controller_)
        {
            sim_controller_->postInit(this);
        }
    }

    void PegasusState::cleanup()
    {
        if (sim_controller_)
        {
            sim_controller_->onSimulationFinished(this);
        }
    }

    void PegasusState::registerWaitOnReservationSet()
    {
        auto m = pegasus_core_->getSystem()->getSystemMemory();
        m->getPostWriteNotificationSource().REGISTER_FOR_THIS(waitOnReservationSet_);
    }

    void PegasusState::unregisterWaitOnReservationSet()
    {
        auto m = pegasus_core_->getSystem()->getSystemMemory();
        m->getPostWriteNotificationSource().DEREGISTER_FOR_THIS(waitOnReservationSet_);
    }

    void PegasusState::waitOnReservationSet_(
        const sparta::memory::BlockingMemoryIFNode::PostWriteAccess & data)
    {
        const auto reservation = pegasus_core_->getReservation(hart_id_);
        if (reservation.isValid() && (reservation.getValue() == data.addr))
        {
            unpauseHart();
            // If the thread triggering the WRS is the last running thread,
            // before triggering thread goes into pause, given current implementation,
            // awakening thread must be set to run in order for similation to proceed.
            pegasus_core_->unpauseHart(hart_id_);

            unregisterWaitOnReservationSet();
            // Cancel the WRS.STO timeout event if scheduled.
            pegasus_core_->cancelWrsstoEvent(hart_id_);
        }
    }

    template bool PegasusState::compare<false>(const PegasusState* rhs) const;
    template bool PegasusState::compare<true>(const PegasusState* rhs) const;

    template bool PegasusState::SimState::compare<false>(const SimState* rhs) const;
    template bool PegasusState::SimState::compare<true>(const SimState* rhs) const;

    void PegasusState::updateCsrEnabledState_()
    {
        // Check for disabled extensions
        for (auto & dep : csr_rset_->getRegisterExtensionDep())
        {
            const auto & reg = dep.first;
            const auto & extensions = dep.second;

            csr_enabled_states_[reg] =
                std::all_of(extensions.begin(), extensions.end(), [this](const std::string & ext)
                            { return extension_manager_.isEnabled(ext); });
        }
    }

    // Install register callback functions
    template <typename XLEN> void PegasusState::addCSRRegisterCallbacks_()
    {
        static_assert(sizeof(XLEN) == 4 || sizeof(XLEN) == 8);

        Rvzicsrind::addCSRRegisterCallbacks<XLEN>(this);
    }

} // namespace pegasus
