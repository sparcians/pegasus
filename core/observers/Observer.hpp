#pragma once

#include "core/Action.hpp"
#include "core/Exception.hpp"
#include "mavis/OpcodeInfo.h"
#include "include/AtlasUtils.hpp"
#include "sparta/functional/Register.hpp"
#include "sparta/memory/BlockingMemoryIFNode.hpp"

namespace atlas
{
    class AtlasState;
    class ActionGroup;

    class Observer
    {
      public:
        Observer() { reset_(); }

        virtual ~Observer() = default;

        struct SrcReg
        {
            SrcReg(const RegId id) : reg_id(id) {}

            SrcReg(const RegId id, uint64_t value) : reg_id(id), reg_value(value) {}

            void setValue(const uint64_t & value) { reg_value = value; }

            const RegId reg_id;
            uint64_t reg_value;
        };

        struct DestReg : SrcReg
        {
            DestReg(const RegId id, uint64_t prev_value) : SrcReg(id), reg_prev_value(prev_value) {}

            DestReg(const RegId id, uint64_t value, uint64_t prev_value) :
                SrcReg(id, value),
                reg_prev_value(prev_value)
            {
            }

            void setPrevValue(const uint64_t & value) { reg_prev_value = value; }

            uint64_t reg_prev_value;
        };

        uint64_t getPrevRdValue() const
        {
            sparta_assert(dst_regs_.size() == 1);
            return dst_regs_[0].reg_prev_value;
        }

        ActionGroup* preExecute(AtlasState* state)
        {
            reset_();
            return preExecute_(state);
        }

        ActionGroup* postExecute(AtlasState* state) { return postExecute_(state); }

        ActionGroup* preException(AtlasState* state) { return preException_(state); }

        virtual void stopSim() {}

        struct MemRead
        {
            Addr addr;
            size_t size;
            uint64_t value;
        };

        struct MemWrite : MemRead
        {
            uint64_t prior_value;
        };

        void registerReadWriteCsrCallbacks(sparta::RegisterBase* reg)
        {
            reg->getPostWriteNotificationSource().REGISTER_FOR_THIS(postCsrWrite_);
            reg->getReadNotificationSource().REGISTER_FOR_THIS(postCsrRead_);
        }

        void registerReadWriteMemCallbacks(sparta::memory::BlockingMemoryIFNode* m)
        {
            m->getPostWriteNotificationSource().REGISTER_FOR_THIS(postMemWrite_);
            m->getReadNotificationSource().REGISTER_FOR_THIS(postMemRead_);
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
        std::map<uint32_t, SrcReg> csr_reads_;
        std::map<uint32_t, DestReg> csr_writes_;

        // Memory reads and writes
        std::vector<MemRead> mem_reads_;
        std::vector<MemWrite> mem_writes_;

        // Exception cause
        sparta::utils::ValidValue<FaultCause> fault_cause_;
        sparta::utils::ValidValue<InterruptCause> interrupt_cause_;

      private:
        virtual ActionGroup* preExecute_(AtlasState*) { return nullptr; }

        virtual ActionGroup* postExecute_(AtlasState*) { return nullptr; }

        virtual ActionGroup* preException_(AtlasState*) { return nullptr; }

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

        // Callbacks
        void postCsrWrite_(const sparta::TreeNode &, const sparta::TreeNode &,
                           const sparta::Register::PostWriteAccess &);
        void postCsrRead_(const sparta::TreeNode &, const sparta::TreeNode &,
                          const sparta::Register::ReadAccess &);
        void postMemWrite_(const sparta::memory::BlockingMemoryIFNode::PostWriteAccess &);
        void postMemRead_(const sparta::memory::BlockingMemoryIFNode::ReadAccess &);
    };

} // namespace atlas
