#pragma once

#include "core/Action.hpp"
#include "core/Exception.hpp"
#include "mavis/OpcodeInfo.h"
#include "include/AtlasUtils.hpp"
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

            SrcReg(const RegId id, std::vector<uint8_t> value) : reg_id(id), reg_value(value) {}

            void setValue(const std::vector<uint8_t> & value) { reg_value = value; }

            const RegId reg_id;
            std::vector<uint8_t> reg_value;
        };

        struct DestReg : SrcReg
        {
            DestReg(const RegId id, std::vector<uint8_t> prev_value) :
                SrcReg(id),
                reg_prev_value(prev_value)
            {
            }

            void setPrevValue(const std::vector<uint8_t> & value) { reg_prev_value = value; }

            std::vector<uint8_t> reg_prev_value;
        };

        uint64_t getPrevRdValue() const
        {
            sparta_assert(dst_regs_.size() == 1);
            return convertFromByteVector<uint64_t>(dst_regs_[0].reg_prev_value);
        }

        virtual ActionGroup* preExecute(AtlasState*) { return nullptr; }

        virtual ActionGroup* postExecute(AtlasState*) { return nullptr; }

        virtual ActionGroup* preException(AtlasState*) { return nullptr; }

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

        void registerFor(sparta::memory::BlockingMemoryIFNode* m)
        {
            m->getPostWriteNotificationSource().REGISTER_FOR_THIS(postWrite_);
            m->getReadNotificationSource().REGISTER_FOR_THIS(postRead_);
        }

        void deregisterFor(sparta::memory::BlockingMemoryIFNode* m)
        {
            m->getPostWriteNotificationSource().DEREGISTER_FOR_THIS(postWrite_);
            m->getReadNotificationSource().DEREGISTER_FOR_THIS(postRead_);
        }

    protected:
        uint64_t pc_;
        uint64_t opcode_;

        // Mavis pointer for getting the disassembly string
        mavis::OpcodeInfo::PtrType opcode_info_;

        std::vector<SrcReg> src_regs_;
        std::vector<DestReg> dst_regs_;

        const MemRead* getMemoryRead_() const
        {
            return mem_read_.isValid() ? &mem_read_.getValue() : nullptr;
        }

        const MemWrite* getMemoryWrite_() const
        {
            return mem_write_.isValid() ? &mem_write_.getValue() : nullptr;
        }

        // Exception cause
        sparta::utils::ValidValue<TrapCauses> trap_cause_;

        virtual void reset_()
        {
            pc_ = 0;
            opcode_ = std::numeric_limits<uint64_t>::max();
            src_regs_.clear();
            dst_regs_.clear();
            trap_cause_.clearValid();
            mem_read_.clearValid();
            mem_write_.clearValid();
        }

    private:
        void postWrite_(const sparta::memory::BlockingMemoryIFNode::PostWriteAccess & data)
        {
            uint64_t prior_val = 0;
            if (data.prior)
            {
                for (size_t i = 0; i < data.size; ++i)
                {
                    prior_val |= static_cast<uint64_t>(data.prior[i]) << (i * 8);
                }
            }

            uint8_t buf[2048];
            data.mem->peek(data.addr, data.size, buf);

            uint64_t final_val = 0;
            for (size_t i = 0; i < data.size; ++i)
            {
                final_val |= static_cast<uint64_t>(buf[i]) << (i * 8);
            }

            MemWrite mem_write;
            mem_write.addr = data.addr;
            mem_write.size = data.size;
            mem_write.value = final_val;
            mem_write.prior_value = prior_val;
            mem_write_ = mem_write;
        }

        void postRead_(const sparta::memory::BlockingMemoryIFNode::ReadAccess & data)
        {
            uint64_t val = 0;
            for (size_t i = 0; i < data.size; ++i)
            {
                val |= static_cast<uint64_t>(data.data[i]) << (i * 8);
            }

            MemRead mem_read;
            mem_read.addr = data.addr;
            mem_read.size = data.size;
            mem_read.value = val;
            mem_read_ = mem_read;
        }

        sparta::utils::ValidValue<MemRead> mem_read_;
        sparta::utils::ValidValue<MemWrite> mem_write_;
    };

} // namespace atlas
