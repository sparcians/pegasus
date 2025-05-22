#pragma once

#include "mavis/OpcodeInfo.h"
#include "sparta/functional/Register.hpp"
#include "sparta/memory/BlockingMemoryIFNode.hpp"
#include "core/Trap.hpp"
#include "include/AtlasTypes.hpp"

namespace atlas
{
    class AtlasState;
    class ActionGroup;

    // The base class needs to know if we are rv32 or rv64 since it is responsible for
    // reading register values (XLEN).
    //
    // We do not use templates here (template <typename XLEN>) because then AtlasState
    // cannot hold onto arch-agnostic observers (std::vector<std::unique_ptr<Observer>>).
    //
    // Note that if your subclass tells the Observer to use ObserverMode::UNUSED, then
    // the before/after register values will NOT be tracked.
    enum class ObserverMode
    {
        RV32,
        RV64,
        UNUSED
    };

    class Observer
    {
      public:
        static uint32_t getRegWidth(const ObserverMode arch)
        {
            switch (arch)
            {
                case ObserverMode::RV32:
                    return 8;
                case ObserverMode::RV64:
                    return 16;
                default:
                    sparta_assert(false, "Invalid architecture");
            }
        }

        uint32_t getRegWidth() const { return Observer::getRegWidth(arch_); }

        Observer(const ObserverMode arch)
        {
            if (arch != ObserverMode::UNUSED)
            {
                arch_ = arch;
            }

            reset_();
        }

        virtual ~Observer() = default;

        struct ObservedReg
        {
            ObservedReg(const RegId id) : reg_id(id) {}

            ObservedReg(const RegId id, uint64_t value) : reg_id(id), reg_value(value) {}

            void setValue(const uint64_t value) { reg_value = value; }

            const RegId reg_id;
            uint64_t reg_value;
        };

        using SrcReg = ObservedReg;

        struct DestReg : ObservedReg
        {
            DestReg(const RegId id, uint64_t prev_value) :
                ObservedReg(id),
                reg_prev_value(prev_value)
            {
            }

            DestReg(const RegId id, uint64_t value, uint64_t prev_value) :
                ObservedReg(id, value),
                reg_prev_value(prev_value)
            {
            }

            void setPrevValue(const uint64_t value) { reg_prev_value = value; }

            uint64_t reg_prev_value;
        };

        void preExecute(AtlasState* state);

        void postExecute(AtlasState* state);

        void preException(AtlasState* state);

        virtual void stopSim() {}

        struct ObservedMemAccess
        {
            Addr addr;
            size_t size;
            uint64_t value;
        };

        using MemRead = ObservedMemAccess;

        struct MemWrite : ObservedMemAccess
        {
            uint64_t prior_value;
        };

        void registerReadWriteCsrCallbacks(sparta::RegisterBase* reg)
        {
            if (arch_.isValid())
            {
                reg->getPostWriteNotificationSource().REGISTER_FOR_THIS(postCsrWrite_);
                reg->getReadNotificationSource().REGISTER_FOR_THIS(postCsrRead_);
            }
        }

        void registerReadWriteMemCallbacks(sparta::memory::BlockingMemoryIFNode* m)
        {
            if (arch_.isValid())
            {
                m->getPostWriteNotificationSource().REGISTER_FOR_THIS(postMemWrite_);
                m->getReadNotificationSource().REGISTER_FOR_THIS(postMemRead_);
            }
        }

      protected:
        uint64_t pc_;
        uint64_t opcode_;

        // Mavis pointer for getting the disassembly string
        mavis::OpcodeInfo::PtrType opcode_info_;

        // Instruction source and destination registers
        std::vector<SrcReg> src_regs_;
        std::vector<DestReg> dst_regs_;

        // Implicit CSR reads and writes
        std::unordered_map<uint32_t, SrcReg> csr_reads_;
        std::unordered_map<uint32_t, DestReg> csr_writes_;

        // Memory reads and writes
        std::vector<MemRead> mem_reads_;
        std::vector<MemWrite> mem_writes_;

        // Exception cause
        sparta::utils::ValidValue<FaultCause> fault_cause_;
        sparta::utils::ValidValue<InterruptCause> interrupt_cause_;

      private:
        sparta::utils::ValidValue<ObserverMode> arch_;

        void inspectInitialState_(AtlasState* state);

        virtual void preExecute_(AtlasState*) {}

        virtual void postExecute_(AtlasState*) {}

        virtual void preException_(AtlasState*) {}

        void reset_()
        {
            pc_ = 0;
            opcode_ = std::numeric_limits<uint64_t>::max();
            src_regs_.clear();
            dst_regs_.clear();
            csr_reads_.clear();
            csr_writes_.clear();
            fault_cause_.clearValid();
            interrupt_cause_.clearValid();
            mem_reads_.clear();
            mem_writes_.clear();
        }

        uint64_t readRegister_(const sparta::Register* reg);

        // Callbacks
        void postCsrWrite_(const sparta::TreeNode &, const sparta::TreeNode &,
                           const sparta::Register::PostWriteAccess &);
        void postCsrRead_(const sparta::TreeNode &, const sparta::TreeNode &,
                          const sparta::Register::ReadAccess &);
        void postMemWrite_(const sparta::memory::BlockingMemoryIFNode::PostWriteAccess &);
        void postMemRead_(const sparta::memory::BlockingMemoryIFNode::ReadAccess &);
    };

} // namespace atlas
