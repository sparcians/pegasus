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
#include "core/observers/SimController.hpp"
#include "core/observers/InstructionLogger.hpp"
#include "core/observers/STFLogger.hpp"
#include "core/observers/STFValidator.hpp"

#include "mavis/mavis/Mavis.h"

#include "sparta/simulation/ResourceTreeNode.hpp"
#include "sparta/memory/SimpleMemoryMapNode.hpp"
#include "sparta/utils/LogUtils.hpp"
#include "sparta/utils/SpartaTester.hpp"

#include <algorithm>

namespace pegasus
{
    uint64_t getInstLimit(sparta::TreeNode* rtn, uint64_t ilimit)
    {
        auto extension = sparta::notNull(rtn->getExtension("sim"));
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
        xlen_(p->xlen),
        reg_json_file_path_(p->reg_json_file_path),
        ilimit_(getInstLimit(hart_tn->getRoot(), p->ilimit)),
        quantum_(p->quantum),
        stop_sim_on_wfi_(p->stop_sim_on_wfi),
        stf_filename_(p->stf_filename),
        validation_stf_filename_(p->validate_with_stf),
        validate_trace_begin_(p->validate_trace_begin),
        validate_inst_begin_(p->validate_inst_begin),
        ulimit_stack_size_(p->ulimit_stack_size),
        priv_mode_(getPrivilegeMode(p->priv_mode)),
        inst_logger_(hart_tn, "inst", "Pegasus Instruction Logger"),
        stf_valid_logger_(hart_tn, "stf_valid", "Pegasus STF Validator Logger"),
        finish_action_group_("finish_inst"),
        stop_sim_action_group_("stop_sim"),
        pause_sim_action_group_("pause_sim")
    {
        // Set up register sets
        int_rset_ = RegisterSet::create(hart_tn, reg_json_file_path_ + std::string("/reg_int.json"),
                                        "int_regs");
        fp_rset_ = RegisterSet::create(hart_tn, reg_json_file_path_ + std::string("/reg_fp.json"),
                                       "fp_regs");
        const std::string vec_reg_json = "/reg_vec" + std::to_string(vlen_) + ".json";
        vec_rset_ = RegisterSet::create(hart_tn, reg_json_file_path_ + vec_reg_json, "vec_regs");
        csr_rset_ = RegisterSet::create(
            hart_tn, reg_json_file_path_ + std::string("/reg_csr_hart.json"), "csr_regs");

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

        add_registers(int_rset_);
        add_registers(fp_rset_);
        add_registers(vec_rset_);
        add_registers(csr_rset_);

        // Increment PC Action
        const bool CHECK_ILIMIT = ilimit_ > 0;
        if (CHECK_ILIMIT)
        {
            increment_pc_action_ = pegasus::Action::createAction<&PegasusState::incrementPc_<true>>(
                this, "increment pc");
        }
        else
        {
            increment_pc_action_ =
                pegasus::Action::createAction<&PegasusState::incrementPc_<false>>(this,
                                                                                  "increment pc");
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
    }

    void PegasusState::onBindTreeLate_()
    {
        // Set up translation
        if (xlen_ == 64)
        {
            updateTranslationMode<RV64>(translate_types::TranslationStage::SUPERVISOR);
            if (pegasus_core_->hasHypervisor())
            {
                updateTranslationMode<RV64>(translate_types::TranslationStage::VIRTUAL_SUPERVISOR);
                updateTranslationMode<RV64>(translate_types::TranslationStage::GUEST);
            }
        }
        else
        {
            updateTranslationMode<RV32>(translate_types::TranslationStage::SUPERVISOR);
            if (pegasus_core_->hasHypervisor())
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
                    validate_trace_begin_, validate_inst_begin_));
            }
            else
            {
                addObserver(std::make_unique<STFValidator>(
                    stf_valid_logger_, ObserverMode::RV32, validation_stf_filename_,
                    validate_trace_begin_, validate_inst_begin_));
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

        const uint32_t mprv_val = READ_CSR_FIELD<XLEN>(this, MSTATUS, "mprv");
        translate_types::TranslationMode ls_mode = mode;
        if (mprv_val == 1)
        {
            if (pegasus_core_->hasHypervisor() == false)
            {
                const PrivMode prev_priv_mode =
                    (PrivMode)READ_CSR_FIELD<XLEN>(this, MSTATUS, "mpp");
                ldst_priv_mode_ = (mprv_val == 1) ? prev_priv_mode : priv_mode_;
                ls_mode = (ldst_priv_mode_ == PrivMode::MACHINE)
                              ? translate_types::TranslationMode::BAREMETAL
                              : mode;
            }
            else
            {
                sparta_assert(false, "Hypervisor does not support MPRV yet!");
            }
        }

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
        if (sim_state_.sim_pause_reason == SimPauseReason::INVALID)
        {
            sim_state_.sim_pause_reason = reason;
            finish_action_group_.setNextActionGroup(&pause_sim_action_group_);
        }
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

    template <typename MemoryType> MemoryType PegasusState::readMemory(const Addr paddr)
    {
        auto* memory = pegasus_core_->getSystem()->getSystemMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        std::vector<uint8_t> buffer(sizeof(MemoryType) / sizeof(uint8_t), 0);
        const bool success = memory->tryRead(paddr, size, buffer.data());
        sparta_assert(success, "Failed to read from memory at address 0x" << std::hex << paddr);

        const MemoryType value = convertFromByteVector<MemoryType>(buffer);
        ILOG("Memory read (" << std::dec << size << "B) to 0x" << std::hex << paddr << ": 0x"
                             << (uint64_t)value);
        return value;
    }

    template <typename MemoryType>
    void PegasusState::writeMemory(const Addr paddr, const MemoryType value)
    {
        auto* memory = pegasus_core_->getSystem()->getSystemMemory();

        static_assert(std::is_trivial<MemoryType>());
        static_assert(std::is_standard_layout<MemoryType>());
        const size_t size = sizeof(MemoryType);
        const std::vector<uint8_t> buffer = convertToByteVector<MemoryType>(value);
        const bool success = memory->tryWrite(paddr, size, buffer.data());
        sparta_assert(success, "Failed to write to memory at address 0x" << std::hex << paddr);

        ILOG("Memory write (" << std::dec << size << "B) to 0x" << std::hex << paddr << ": 0x"
                              << (uint64_t)value);
    }

    template int8_t PegasusState::readMemory<int8_t>(const Addr);
    template uint8_t PegasusState::readMemory<uint8_t>(const Addr);
    template int16_t PegasusState::readMemory<int16_t>(const Addr);
    template uint16_t PegasusState::readMemory<uint16_t>(const Addr);
    template int32_t PegasusState::readMemory<int32_t>(const Addr);
    template uint32_t PegasusState::readMemory<uint32_t>(const Addr);
    template int64_t PegasusState::readMemory<int64_t>(const Addr);
    template uint64_t PegasusState::readMemory<uint64_t>(const Addr);
    template void PegasusState::writeMemory<uint8_t>(const Addr, const uint8_t);
    template void PegasusState::writeMemory<uint16_t>(const Addr, const uint16_t);
    template void PegasusState::writeMemory<uint32_t>(const Addr, const uint32_t);
    template void PegasusState::writeMemory<uint64_t>(const Addr, const uint64_t);

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

    template <bool CHECK_ILIMIT>
    Action::ItrType PegasusState::incrementPc_(PegasusState*, Action::ItrType action_it)
    {
        // Set PC
        prev_pc_ = pc_;
        pc_ = next_pc_;
        DLOG("PC: 0x" << std::hex << pc_);

        // Increment instruction count
        ++sim_state_.inst_count;

        // TODO: We don't have a timing model yet so
        // for now just assume each inst takes 1 cycle
        ++sim_state_.cycles;

        if constexpr (CHECK_ILIMIT)
        {
            if (sim_state_.inst_count == ilimit_)
            {
                std::cout << "Reached instruction limit (" << std::dec << ilimit_
                          << "), stopping simulation." << std::endl;
                const uint64_t exit_code = 0;
                stopSim(exit_code);
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
        auto xlen = getXlen();
        auto other_xlen = state->getXlen();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(xlen, other_xlen);
        }
        else if (xlen != other_xlen)
        {
            return false;
        }

        auto stop_sim_on_wfi = getStopSimOnWfi();
        auto other_stop_sim_on_wfi = state->getStopSimOnWfi();
        if constexpr (IS_UNIT_TEST)
        {
            EXPECT_EQUAL(stop_sim_on_wfi, other_stop_sim_on_wfi);
        }
        else if (stop_sim_on_wfi != other_stop_sim_on_wfi)
        {
            return false;
        }

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
        if (pegasus_core_->isExtensionEnabled("f"))
        {
            a_val |= COMPAT_HWCAP_ISA_F;
        }
        if (pegasus_core_->isExtensionEnabled("d"))
        {
            a_val |= COMPAT_HWCAP_ISA_D;
        }
        if (pegasus_core_->isExtensionEnabled("c"))
        {
            a_val |= COMPAT_HWCAP_ISA_C;
        }
        if (pegasus_core_->isExtensionEnabled("v"))
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

                const uint32_t ext_val = pegasus_core_->getMisaExtFieldValue<RV64>();
                POKE_CSR_FIELD<RV64>(this, MISA, "extensions", ext_val);

                // Initialize MSTATUS/STATUS with User and Supervisor mode XLEN
                POKE_CSR_FIELD<RV64>(this, MSTATUS, "uxl", xlen_val);
                POKE_CSR_FIELD<RV64>(this, MSTATUS, "sxl", xlen_val);
                POKE_CSR_FIELD<RV64>(this, SSTATUS, "uxl", xlen_val);
            }
            else
            {
                POKE_CSR_REG<RV32>(this, MHARTID, hart_id_);

                const uint32_t xlen_val = 1;
                POKE_CSR_FIELD<RV32>(this, MISA, "mxl", xlen_val);

                const uint32_t ext_val = pegasus_core_->getMisaExtFieldValue<RV32>();
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

    template bool PegasusState::compare<false>(const PegasusState* rhs) const;
    template bool PegasusState::compare<true>(const PegasusState* rhs) const;

    template bool PegasusState::SimState::compare<false>(const SimState* rhs) const;
    template bool PegasusState::SimState::compare<true>(const SimState* rhs) const;

} // namespace pegasus
