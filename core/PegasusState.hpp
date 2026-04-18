#pragma once

#include "core/ActionGroup.hpp"
#include "core/PegasusInst.hpp"
#include "core/observers/Observer.hpp"
#include "core/VecConfig.hpp"

#include "arch/RegisterSet.hpp"
#include "arch/gen/supportedISA.hpp"
#include "include/PegasusTypes.hpp"
#include "include/gen/CSRBitMasks64.hpp"
#include "include/gen/CSRBitMasks32.hpp"
#include "include/gen/CSRHelpers.hpp"

#include "sim/PegasusAllocators.hpp"

#include "sparta/simulation/ParameterSet.hpp"
#include "sparta/simulation/Unit.hpp"
#include "sparta/utils/SpartaSharedPointerAllocator.hpp"
#include "core/PegasusAllocatorWrapper.hpp"

#include "mavis/extension_managers/RISCVExtensionManager.hpp"

#include <filesystem>
#include <regex>

template <class InstT, class ExtenT, class InstTypeAllocator, class ExtTypeAllocator> class Mavis;

namespace pegasus
{
    class PegasusInst;
    using PegasusInstPtr = sparta::SpartaSharedPointer<PegasusInst>;
    class PegasusCore;
    class PegasusSystem;
    class Fetch;
    class Execute;
    class Translate;
    class Exception;
    class SimController;
    class VectorState;
    class STFLogger;
    class STFValidator;
    class SystemCallEmulator;

    namespace cosim
    {
        class Event;
    }

    using MavisType =
        Mavis<PegasusInst, PegasusExtractor, PegasusInstAllocatorWrapper<PegasusInstAllocator>,
              PegasusExtractorAllocatorWrapper<PegasusExtractorAllocator>>;

    class PegasusState : public sparta::Unit
    {
      public:
        // Name of this resource, required by sparta::UnitFactory
        static constexpr char name[] = "PegasusState";
        using base_type = PegasusState;

        class PegasusStateParameters : public sparta::ParameterSet
        {
          public:
            PegasusStateParameters(sparta::TreeNode* node) : sparta::ParameterSet(node)
            {
                vlen.addDependentValidationCallback(&PegasusStateParameters::validateVlen_,
                                                    "VLEN constraint");
            }

            PARAMETER(uint32_t, hart_id, UINT32_MAX, "Hart ID")
            PARAMETER(char, priv_mode, 'm', "Privilege mode at boot (m, s, or u)")
            PARAMETER(std::string, isa, "",
                      "ISA string when hart boots. If not set, the ISA string from PegasusCore is "
                      "used instead.")
            PARAMETER(uint32_t, vlen, 256, "Vector register size in bits (max: 1024)")
            PARAMETER(uint32_t, init_lmul, 8,
                      "Initial vector LMUL in units of 1/8 (e.g. 8=1, 16=2, 4=1/2)")
            PARAMETER(uint32_t, init_sew, 8, "Initial vector SEW in bits")
            PARAMETER(bool, init_vta, false, "Initial vector tail agnostic state")
            PARAMETER(bool, init_vma, false, "Initial vector mask agnostic state")
            PARAMETER(uint32_t, init_vl, 0, "Initial vector length (VL)")
            PARAMETER(uint32_t, init_vstart, 0, "Initial vector start index (VSTART)")
            PARAMETER(uint32_t, ilimit, 0, "Instruction limit for stopping simulation")
            PARAMETER(uint32_t, quantum, 500, "Instruction quantum size")
            PARAMETER(bool, stop_sim_on_wfi, false, "Executing a WFI instruction stops simulation")
            // Typical stack pointer is 8KB on most linux systems
            PARAMETER(uint32_t, ulimit_stack_size, 8192,
                      "Typical ulimit stack size for system call emulation")

            // STF Validation
            PARAMETER(std::string, stf_filename, "",
                      "STF Trace file name (when not given, STF tracing is disabled)")
            PARAMETER(std::string, validate_with_stf, "",
                      "STF Trace file name (when not given, STF tracing is disabled)")
            PARAMETER(uint64_t, validate_trace_begin, 1,
                      "STF validation trace file begin instruction number")
            PARAMETER(uint64_t, validate_inst_begin, 1,
                      "STF validation pegasus begin instruction number")
            PARAMETER(bool, validate_fail_on_first_diff, false,
                      "STF validation pegasus fail on first difference detected")

            // Set by PegasusCore
            HIDDEN_PARAMETER(std::string, reg_json_file_path, "",
                             "Where are the Pegasus register files?")
          private:
            static bool validateVlen_(uint32_t & vlen_val, const sparta::TreeNode*)
            {
                const std::vector<uint32_t> valid_vlen_values{128, 256, 512, 1024};
                return std::find(valid_vlen_values.begin(), valid_vlen_values.end(), vlen_val)
                       != valid_vlen_values.end();
            }
        };

        PegasusState(sparta::TreeNode* core_node, const PegasusStateParameters* p);

        // Not default -- defined in source file to reduce massive inlining
        virtual ~PegasusState();

        HartId getHartId() const { return hart_id_; }

        const std::string & getISAString() const { return isa_string_; }

        uint64_t getXlen() const;

        uint64_t getQuantumSize() const { return quantum_; }

        bool getStopSimOnWfi() const { return stop_sim_on_wfi_; }

        void setPc(Addr pc) { pc_ = pc; }

        Addr getPc() const { return pc_; }

        Addr getPrevPc() const { return prev_pc_; }

        void setNextPc(Addr next_pc) { next_pc_ = next_pc; }

        Addr getNextPc() const { return next_pc_; }

        uint64_t getPcAlignment() const { return pc_alignment_; }

        uint64_t getPcAlignmentMask() const { return pc_alignment_mask_; }

        PrivMode getPrivMode() const { return priv_mode_; }

        PrivMode getLdstPrivMode(translate_types::TranslationStage stage =
                                     translate_types::TranslationStage::SUPERVISOR) const
        {
            return ldst_priv_modes_.at(static_cast<uint32_t>(stage));
        }

        bool getVirtualMode() const { return virtual_mode_; }

        void setPrivMode(PrivMode priv_mode, bool virt_mode);

        using Reservation = sparta::utils::ValidValue<Addr>;

        template <typename XLEN>
        void updateTranslationMode(const translate_types::TranslationStage);

        struct SimState
        {
            // Executing instruction
            uint32_t current_opcode = 0;
            uint64_t current_uid = 0;
            PegasusInstPtr current_inst = nullptr;

            // Number of instructions executed
            uint64_t inst_count = 0;

            // How many cycles
            uint64_t cycles = 0;

            // Simulation control
            SimPauseReason sim_pause_reason = SimPauseReason::INVALID;
            bool sim_stopped = true;
            bool test_passed = true;
            int64_t workload_exit_code = 0;

            void reset()
            {
                current_opcode = 0;
                current_inst.reset();
                ++current_uid;
            }

            template <bool IS_UNIT_TEST = false> bool compare(const SimState* state) const;
        };

        const SimState* getSimState() const { return &sim_state_; }

        SimState* getSimState() { return &sim_state_; }

        void pauseHart(const SimPauseReason reason);

        void unpauseHart();

        const VectorConfig* getVectorConfig() const { return &vector_config_; }

        VectorConfig* getVectorConfig() { return &vector_config_; }

        const PegasusInstPtr & getCurrentInst() { return sim_state_.current_inst; }

        void setCurrentInst(PegasusInstPtr inst)
        {
            inst->setUid(sim_state_.current_uid);
            sim_state_.current_inst = inst;
        }

        void throwException(FaultCause cause)
        {
            auto exception_unit = getExceptionUnit();
            exception_unit->setUnhandledException(cause);
            throw ActionException(exception_unit->getActionGroup());
        }

        void setCurrentException(uint64_t excp_code) { current_exception_ = excp_code; }

        void clearCurrentException() { current_exception_ = std::numeric_limits<ExcpCode>::max(); }

        uint64_t getCurrentException() const { return current_exception_; }

        PegasusTranslationState* getFetchTranslationState() { return &fetch_translation_state_; }

        PegasusTranslationState* getInstTranslationState() { return &inst_translation_state_; }

        PegasusCore* getCore() const { return pegasus_core_; }

        void setPegasusCore(PegasusCore* pegasus_core) { pegasus_core_ = pegasus_core; }

        MavisType* getMavis() { return mavis_.get(); }

        const MavisType* getMavis() const { return mavis_.get(); }

        enum MavisUIDs : mavis::InstructionUniqueID
        {
            MAVIS_UID_CSRRW = 1,
            MAVIS_UID_CSRRS,
            MAVIS_UID_CSRRC,
            MAVIS_UID_CSRRWI,
            MAVIS_UID_CSRRSI,
            MAVIS_UID_CSRRCI,
            MAVIS_UID_HLVX_HU,
            MAVIS_UID_HLVX_WU
        };

        void changeMavisContext();

        mavis::extension_manager::riscv::RISCVExtensionManager & getExtensionManager()
        {
            return extension_manager_;
        }

        const mavis::extension_manager::riscv::RISCVExtensionManager & getExtensionManager() const
        {
            return extension_manager_;
        }

        bool isExtensionEnabled(std::string ext) const { return extension_manager_.isEnabled(ext); }

        bool isCompressionEnabled() const { return extension_manager_.isEnabled("zca"); }

        bool hasHypervisor() const { return hypervisor_enabled_; }

        bool hasZicntr() const { return zicntr_enabled_; }

        template <typename XLEN> uint32_t getMisaExtFieldValue() const;

        void enableInteractiveMode();

        void useSpikeFormatting();

        Fetch* getFetchUnit() const { return fetch_unit_; }

        Execute* getExecuteUnit() const { return execute_unit_; }

        Translate* getTranslateUnit() const { return translate_unit_; }

        sparta::Register* getSpartaRegister(const mavis::OperandInfo::Element* operand);

        pegasus::RegisterSet* getIntRegisterSet() { return int_rset_.get(); }

        pegasus::RegisterSet* getFpRegisterSet() { return fp_rset_.get(); }

        pegasus::RegisterSet* getVecRegisterSet() { return vec_rset_.get(); }

        pegasus::RegisterSet* getCsrRegisterSet() { return csr_rset_.get(); }

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

        sparta::Register* findRegister(const RegId reg_id)
        {
            switch (reg_id.reg_type)
            {
                case RegType::INTEGER:
                    return getIntRegister(reg_id.reg_num);
                case RegType::FLOATING_POINT:
                    return getFpRegister(reg_id.reg_num);
                case RegType::VECTOR:
                    return getVecRegister(reg_id.reg_num);
                case RegType::CSR:
                    return getCsrRegister(reg_id.reg_num);
                case RegType::INVALID:
                    sparta_assert(false, "Invalid register type!");
            }
            return nullptr;
        }

        sparta::Register* findRegister(const std::string & reg_name, bool must_exist = true) const;

        inline bool isRegEnabled(uint32_t id) const
        {
            if (csr_enabled_states_.contains(id))
            {
                return csr_enabled_states_.at(id);
            }
            return true;
        }

        // Memory supplement for observing memory reads and writes
        struct MemorySupplement
        {
            const Addr paddr;
            const Addr vaddr;
            const MemAccessSource source;

            MemorySupplement(Addr paddr, Addr vaddr, const MemAccessSource source) :
                paddr(paddr),
                vaddr(vaddr),
                source(source)
            {
            }
        };

        template <typename MemoryType>
        bool readMemory(const PegasusTranslationState::TranslationResult & result,
                        std::vector<uint8_t> & buffer,
                        const MemAccessSource source = MemAccessSource::INVALID);
        template <typename MemoryType>
        bool readMemory(const Addr paddr, std::vector<uint8_t> & buffer,
                        const MemAccessSource source = MemAccessSource::INVALID);
        template <typename MemoryType>
        bool writeMemory(const PegasusTranslationState::TranslationResult & result,
                         const MemoryType value,
                         const MemAccessSource source = MemAccessSource::INVALID);
        template <typename MemoryType>
        bool writeMemory(const Addr paddr, const MemoryType value,
                         const MemAccessSource source = MemAccessSource::INVALID);

        void addObserver(std::unique_ptr<Observer> observer);

        const std::vector<std::unique_ptr<Observer>> & getObservers() const { return observers_; }

        void insertExecuteActions(ActionGroup* action_group, const bool is_memory_inst);

        ActionGroup* getFinishActionGroup() { return &finish_action_group_; }

        ActionGroup* getStopSimActionGroup() { return &stop_sim_action_group_; }

        Exception* getExceptionUnit() const { return exception_unit_; }

        void setSimStopped(bool sim_stopped, const int64_t exit_code = 0);

        template <bool IS_UNIT_TEST = false> bool compare(const PegasusState* state) const;

        template <typename XLEN> XLEN emulateSystemCall();

        // Initialze a program stack (argc, argv, envp, auxv, etc)
        void setupProgramStack(const std::vector<std::string> & program_arguments);

        // For standalone Pegasus simulations, this method will be
        // called at the top of PegasusSim::run()
        void boot();

        // One-time cleanup phase after simulation end.
        void cleanup();

        // Register a WaitOnReservationSet notification.
        void registerWaitOnReservationSet();
        // Unregister a WaitOnReservationSet notification.
        void unregisterWaitOnReservationSet();

        bool storeOnReservationSetOccurred() const { return store_on_resvset_; };

        void storeOnReservationSet(bool occurred) { store_on_resvset_ = occurred; }

      private:
        void onBindTreeEarly_() override;
        void onBindTreeLate_() override;

        Action::ItrType preExecute_(PegasusState* state, Action::ItrType action_it);
        Action::ItrType postExecute_(PegasusState* state, Action::ItrType action_it);
        Action::ItrType preException_(PegasusState* state, Action::ItrType action_it);

        Action pre_execute_action_;
        Action post_execute_action_;
        Action pre_exception_action_;

        Action::ItrType stopSim_(PegasusState*, Action::ItrType action_it)
        {
            for (auto & obs : observers_)
            {
                obs->stopSim();
            }

            return ++action_it;
        }

        Action::ItrType pauseSim_(PegasusState*, Action::ItrType action_it) { return ++action_it; }

        void
        waitOnReservationSet_(const sparta::memory::BlockingMemoryIFNode::PostWriteAccess & data);

        /*!
         *  \brief Installs register read/write callback functions to special registers
         */
        template <typename XLEN> void addCSRRegisterCallbacks_();

        //! Hart ID
        const HartId hart_id_;

        // VLEN (128, 256, 512, 1024 or 2048 bits)
        const uint32_t vlen_;

        // Path to register JSONs
        const std::string reg_json_file_path_;

        // Instruction limit to end simulation
        const uint64_t ilimit_ = 0;

        // Instruction quantum size
        const uint64_t quantum_;

        //! Stop simulatiion on WFI
        const bool stop_sim_on_wfi_;

        //! Typical stack size for system call emulation
        const uint64_t ulimit_stack_size_;

        // STF Trace Filename
        const std::string stf_filename_;
        const std::string validation_stf_filename_;
        const uint64_t validate_trace_begin_;
        const uint64_t validate_inst_begin_;
        const bool validate_fail_on_first_diff_;

        //! Current pc
        Addr pc_ = 0x0;

        //! Next pc
        Addr next_pc_ = 0x0;

        //! Previous pc
        Addr prev_pc_ = 0x0;

        //! PC alignment
        uint64_t pc_alignment_ = 4;

        //! PC alignment
        uint64_t pc_alignment_mask_ = ~(pc_alignment_ - 1);

        void setPcAlignment_()
        {
            pc_alignment_ = isCompressionEnabled() ? 2 : 4;
            pc_alignment_mask_ = ~(pc_alignment_ - 1);
        }

        //! Current privilege mode
        PrivMode priv_mode_ = PrivMode::MACHINE;

        //! Current privilege mode for LS translation
        std::array<PrivMode, translate_types::N_TRANS_STAGES> ldst_priv_modes_{PrivMode::MACHINE};

        //! Current virtual translation mode
        bool virtual_mode_ = false;

        //! Current exception code
        uint64_t current_exception_ = std::numeric_limits<ExcpCode>::max();

        //! Simulation state
        SimState sim_state_;

        //! Vector state
        VectorConfig vector_config_;

        // Increment counter CSRs
        template <typename XLEN> void incrCycleCsrs_();
        template <typename XLEN> void incrTimeCsrs_();
        template <typename XLEN> void incrInstretCsrs_();

        // Increment PC Action
        template <typename XLEN, bool CHECK_ILIMIT>
        Action::ItrType incrementPc_(PegasusState* state, Action::ItrType action_it);
        pegasus::Action increment_pc_action_;

        // Fetch translation state
        PegasusTranslationState fetch_translation_state_;

        // Instruction translation state
        PegasusTranslationState inst_translation_state_;

        //! PegasusCore
        PegasusCore* pegasus_core_ = nullptr;

        // ISA string
        std::string isa_string_;

        // XLEN (either 32 or 64 bit)
        uint64_t xlen_ = 64;

        // TODO
        const std::string isa_file_path_;

        // Arch name
        // FIXME: We should be able to get this from Sparta --arch
        const std::string arch_name_;

        // Path to Pegasus uarch JSONs
        const std::string uarch_file_path_;

        // Get Pegasus arch JSONs for Mavis
        mavis::FileNameListType getUArchFiles_() const;

        // Mavis extension manager
        mavis::extension_manager::riscv::RISCVExtensionManager extension_manager_;

        static inline mavis::InstUIDList mavis_uid_list_{
            {"csrrw", MAVIS_UID_CSRRW},     {"csrrs", MAVIS_UID_CSRRS},
            {"csrrc", MAVIS_UID_CSRRC},     {"csrrwi", MAVIS_UID_CSRRWI},
            {"csrrsi", MAVIS_UID_CSRRSI},   {"csrrci", MAVIS_UID_CSRRCI},
            {"hlvx.hu", MAVIS_UID_HLVX_HU}, {"hlvx.wu", MAVIS_UID_HLVX_WU}};

        // Mavis
        std::unique_ptr<MavisType> mavis_;

        //! Do we have hypervisor?
        bool hypervisor_enabled_;

        //! Do we have the counter extension?
        bool zicntr_enabled_;

        // Fetch Unit
        Fetch* fetch_unit_ = nullptr;

        // Execute Unit
        Execute* execute_unit_ = nullptr;

        // Translate Unit
        Translate* translate_unit_ = nullptr;

        // Exception Unit
        Exception* exception_unit_ = nullptr;

        // Register set holding all Sparta registers from all generated JSON files
        std::unique_ptr<RegisterSet> int_rset_;
        std::unique_ptr<RegisterSet> fp_rset_;
        std::unique_ptr<RegisterSet> vec_rset_;
        std::unique_ptr<RegisterSet> csr_rset_;

        // Cached registers by name
        std::unordered_map<std::string, sparta::Register*> registers_by_name_;

        // Track which CSRs are enabled/disabled
        std::map<uint32_t, bool> csr_enabled_states_;
        void updateCsrEnabledState_();

        // Observers
        std::vector<std::unique_ptr<Observer>> observers_;

        // MessageSource used for InstructionLogger
        sparta::log::MessageSource inst_logger_;

        // MessageSource used for STFValidator
        sparta::log::MessageSource stf_valid_logger_;

        // Finish ActionGroup for post-execute simulator Actions
        ActionGroup finish_action_group_;

        // Stop simulation Action
        Action stop_action_;
        ActionGroup stop_sim_action_group_;

        // Pause simulation Action
        Action pause_action_;
        ActionGroup pause_sim_action_group_;

        // Co-simulation debug utils
        std::unordered_map<std::string, int> reg_ids_by_name_;
        SimController* sim_controller_ = nullptr;

        // Whether a store has occured on reservation set.
        bool store_on_resvset_ = false;

        // Event friend class for cosim. Allows direct state manipulation
        // during flush (rollback) operations.
        friend class cosim::Event;
    };

    template <typename XLEN>
    static inline XLEN READ_INT_REG(PegasusState* state, uint32_t reg_ident)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        return (reg_ident == 0) ? 0 : state->getIntRegister(reg_ident)->dmiRead<XLEN>();
    }

    template <typename XLEN>
    static inline void WRITE_INT_REG(PegasusState* state, uint32_t reg_ident, uint64_t reg_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if (reg_ident != 0)
        {
            state->getIntRegister(reg_ident)->dmiWrite<XLEN>(reg_value);
        }
    }

    template <typename XLEN> static inline XLEN READ_FP_REG(PegasusState* state, uint32_t reg_ident)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        return state->getFpRegister(reg_ident)->dmiRead<XLEN>();
    }

    template <typename XLEN>
    static inline void WRITE_FP_REG(PegasusState* state, uint32_t reg_ident, uint64_t reg_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        state->getFpRegister(reg_ident)->dmiWrite<XLEN>(reg_value);
    }

    template <typename VLEN>
    static inline VLEN READ_VEC_REG(PegasusState* state, uint32_t reg_ident)
    {
        return state->getVecRegister(reg_ident)->dmiRead<VLEN>();
    }

    template <typename VLEN>
    static inline void WRITE_VEC_REG(PegasusState* state, uint32_t reg_ident, VLEN reg_value)
    {
        state->getVecRegister(reg_ident)->dmiWrite<VLEN>(reg_value);
    }

    template <typename Elem>
    static inline Elem READ_VEC_ELEM(PegasusState* state, uint32_t reg_ident, uint32_t idx)
    {
        return state->getVecRegister(reg_ident)->dmiRead<Elem>(idx);
    }

    template <typename Elem>
    static inline void WRITE_VEC_ELEM(PegasusState* state, uint32_t reg_ident, Elem value,
                                      uint32_t idx)
    {
        state->getVecRegister(reg_ident)->dmiWrite<Elem>(value, idx);
    }

    template <typename XLEN>
    static inline XLEN READ_CSR_REG(PegasusState* state, uint32_t reg_ident)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        if (SPARTA_EXPECT_FALSE(!state->isRegEnabled(reg_ident)))
        {
            state->throwException(FaultCause::ILLEGAL_INST);
        }

        return state->getCsrRegister(reg_ident)->readWithCheck();
    }

    template <typename XLEN>
    static inline void WRITE_CSR_REG(PegasusState* state, uint32_t reg_ident, uint64_t reg_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        if (SPARTA_EXPECT_FALSE(!state->isRegEnabled(reg_ident)))
        {
            state->throwException(FaultCause::ILLEGAL_INST);
        }

        if (const auto mask = pegasus::getCsrBitMask<XLEN>(reg_ident);
            mask != std::numeric_limits<XLEN>::max())
        {
            auto reg = state->getCsrRegister(reg_ident);
            const auto old_value = reg->dmiRead<XLEN>();
            const auto write_val = (old_value & ~mask) | (reg_value & mask);
            reg->writeWithCheck<XLEN>(write_val);
        }
        else
        {
            state->getCsrRegister(reg_ident)->writeWithCheck<XLEN>(reg_value);
        }
    }

    template <typename XLEN>
    static inline XLEN PEEK_CSR_REG(PegasusState* state, uint32_t reg_ident)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        return state->getCsrRegister(reg_ident)->dmiRead<XLEN>();
    }

    template <typename XLEN>
    static inline void POKE_CSR_REG(PegasusState* state, uint32_t reg_ident, uint64_t reg_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        state->getCsrRegister(reg_ident)->dmiWrite<XLEN>(reg_value);
    }

    template <typename XLEN>
    static inline XLEN READ_CSR_FIELD(PegasusState* state, uint32_t reg_ident,
                                      const char* field_name)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        const auto & csr_bit_range = pegasus::getCsrBitRange<XLEN>(reg_ident, field_name);
        const XLEN field_lsb = csr_bit_range.first;
        const XLEN field_msb = csr_bit_range.second;
        const XLEN max_msb = std::is_same_v<XLEN, RV64> ? 63 : 31;
        // If field spans entire register, no mask/shift is needed
        if ((field_lsb == 0) && (field_msb >= max_msb))
        {
            return state->getCsrRegister(reg_ident)->dmiRead<XLEN>();
        }
        else
        {
            return ((state->getCsrRegister(reg_ident)->dmiRead<XLEN>() >> field_lsb)
                    & ((XLEN(1) << (field_msb - field_lsb + 1)) - 1));
        }
    }

    template <typename XLEN>
    static inline void WRITE_CSR_FIELD(PegasusState* state, uint32_t reg_ident,
                                       const char* field_name, uint64_t field_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        XLEN csr_value = PEEK_CSR_REG<XLEN>(state, reg_ident);

        const auto & csr_bit_range = pegasus::getCsrBitRange<XLEN>(reg_ident, field_name);
        const XLEN field_lsb = csr_bit_range.first;
        const XLEN field_msb = csr_bit_range.second;
        const XLEN max_msb = std::is_same_v<XLEN, RV64> ? 63 : 31;
        // If field spans entire register, no mask/shift is needed
        if ((field_lsb == 0) && (field_msb >= max_msb))
        {
            WRITE_CSR_REG<XLEN>(state, reg_ident, field_value);
        }
        else
        {
            const XLEN mask = ((XLEN(1) << (field_msb - field_lsb + 1)) - 1) << field_lsb;
            csr_value &= ~mask;

            const XLEN new_field_value = field_value << field_lsb;
            csr_value |= new_field_value;

            WRITE_CSR_REG<XLEN>(state, reg_ident, csr_value);
        }
    }

    template <typename XLEN>
    static inline void POKE_CSR_FIELD(PegasusState* state, uint32_t reg_ident,
                                      const char* field_name, uint64_t field_value)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        XLEN csr_value = PEEK_CSR_REG<XLEN>(state, reg_ident);

        const auto & csr_bit_range = pegasus::getCsrBitRange<XLEN>(reg_ident, field_name);
        const XLEN field_lsb = csr_bit_range.first;
        const XLEN field_msb = csr_bit_range.second;
        const XLEN max_msb = std::is_same_v<XLEN, RV64> ? 63 : 31;
        // If field spans entire register, no mask/shift is needed
        if ((field_lsb == 0) && (field_msb >= max_msb))
        {
            POKE_CSR_REG<XLEN>(state, reg_ident, field_value);
        }
        else
        {
            const XLEN mask = ((XLEN(1) << (field_msb - field_lsb + 1)) - 1) << field_lsb;
            csr_value &= ~mask;

            const XLEN new_field_value = field_value << field_lsb;
            csr_value |= new_field_value;

            POKE_CSR_REG<XLEN>(state, reg_ident, csr_value);
        }
    }
} // namespace pegasus
