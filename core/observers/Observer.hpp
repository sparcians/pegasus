#pragma once

#include "mavis/OpcodeInfo.h"
#include "sparta/functional/Register.hpp"
#include "sparta/memory/BlockingMemoryIFNode.hpp"
#include "core/Trap.hpp"
#include "include/PegasusTypes.hpp"

namespace pegasus
{
    class PegasusState;
    class ActionGroup;

    // The base class needs to know if we are rv32 or rv64 since it is responsible for
    // reading register values (XLEN).
    //
    // We do not use templates here (template <typename XLEN>) because then PegasusState
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

        // Holds a register's value as a byte vector
        class ObservedValue
        {
          public:
            ObservedValue() = default;

            template <typename TYPE> ObservedValue(const std::vector<TYPE> & value)
            {
                setValue(value);
            }

            template <std::integral TYPE> ObservedValue(const TYPE & value) { setValue(value); }

            ObservedValue(const ObservedValue &) = default;

            ObservedValue(ObservedValue &&) = default;

            template <typename TYPE> void setValue(const std::vector<TYPE> & value)
            {
                if constexpr (std::is_same_v<TYPE, uint8_t>)
                {
                    value_ = std::move(value);
                }
                else
                {
                    value_.resize(value.size() * sizeof(TYPE));
                    ::memcpy(value_.data(), value.data(), value_.capacity());
                }
            }

            template <std::integral TYPE> void setValue(const TYPE & value)
            {
                value_.resize(sizeof(TYPE));
                ::memcpy(value_.data(), &value, sizeof(TYPE));
            }

            template <typename TYPE> TYPE getValue() const
            {
                if constexpr (std::is_same_v<TYPE, std::vector<uint64_t>>)
                {
                    TYPE value(value_.size() / 8);
                    ::memcpy(value.data(), value_.data(), value_.capacity());
                    return value;
                }
                else
                {
                    TYPE value;
                    ::memcpy(&value, value_.data(), sizeof(TYPE));
                    return value;
                }
                return TYPE();
            }

            size_t size() const { return value_.size(); }

            const std::vector<uint8_t> & getByteVector() const { return value_; }

          private:
            std::vector<uint8_t> value_;

            friend std::ostream & operator<<(std::ostream & os, const ObservedValue & value);
        };

        struct ObservedReg
        {
            ObservedReg(const RegId & id) : reg_id(id) {}

            template <typename TYPE>
            ObservedReg(const RegId & id, TYPE value) : reg_id(id), reg_value_(value)
            {
            }

            template <typename TYPE> TYPE getRegValue() const
            {
                return reg_value_.getValue<TYPE>();
            }

            const ObservedValue & getObservedValue() const { return reg_value_; }

            const RegId reg_id;

          private:
            // Encapsulate the register value since it's non-const
            ObservedValue reg_value_;
        };

        using SrcReg = ObservedReg;

        // A DestReg is an ObservedReg with a new value
        struct DestReg : ObservedReg
        {
            template <typename TYPE> DestReg(const RegId & id, TYPE value) : ObservedReg(id, value)
            {
            }

            template <typename TYPE>
            DestReg(const RegId id, const TYPE & value, const TYPE & new_value) :
                ObservedReg(id, value),
                new_value_(new_value)
            {
            }

            template <typename TYPE> void setNewValue(const TYPE & new_value)
            {
                new_value_.setValue(new_value);
            }

            template <typename TYPE> TYPE getNewValue() const
            {
                return new_value_.getValue<TYPE>();
            }

            const ObservedValue & getObservedNewValue() const { return new_value_; }

          private:
            ObservedValue new_value_;
        };

        void preExecute(PegasusState* state);

        void postExecute(PegasusState* state);

        void preException(PegasusState* state);

        virtual void stopSim() {}

        struct ObservedMemAccess
        {
            template <typename TYPE>
            ObservedMemAccess(const Addr paddr, const Addr vaddr, const size_t size,
                              const TYPE value, const MemAccessSource source) :
                paddr(paddr),
                vaddr(vaddr),
                size(size),
                mem_value(value),
                source(source)
            {
            }

            ObservedMemAccess(const ObservedMemAccess & other) :
                paddr(other.paddr),
                vaddr(other.vaddr),
                size(other.size),
                mem_value(other.mem_value),
                source(other.source)
            {
            }

            ObservedMemAccess & operator=(ObservedMemAccess other)
            {
                std::swap(*this, other);
                return *this;
            }

            template <typename TYPE> TYPE getMemValue() const { return mem_value.getValue<TYPE>(); }

            const Addr paddr;
            const Addr vaddr;
            const size_t size;
            const ObservedValue mem_value;
            const MemAccessSource source;
        };

        using MemRead = ObservedMemAccess;

        struct MemWrite : ObservedMemAccess
        {
            template <typename TYPE>
            MemWrite(const Addr paddr, const Addr vaddr, const size_t size, const TYPE value,
                     const TYPE prev_value, const MemAccessSource source) :
                ObservedMemAccess(paddr, vaddr, size, value, source),
                mem_prev_value(prev_value)
            {
            }

            MemWrite(const MemWrite & other) :
                ObservedMemAccess(other.paddr, other.vaddr, other.size, other.mem_value,
                                  other.source),
                mem_prev_value(other.mem_prev_value)
            {
            }

            MemWrite & operator=(MemWrite other)
            {
                std::swap(*this, other);
                return *this;
            }

            template <typename TYPE> TYPE getMemPrevValue() const
            {
                return mem_prev_value.getValue<TYPE>();
            }

            const ObservedValue mem_prev_value;
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

        const std::vector<MemRead> & getMemoryReads() const { return mem_reads_; }

        const std::vector<MemWrite> & getMemoryWrites() const { return mem_writes_; }

      protected:
        uint64_t pc_;
        PrivMode priv_mode_;
        bool virtual_mode_;
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

        template <typename T> T readScalarRegister_(PegasusState* state, RegId reg_id) const
        {
            switch (reg_id.reg_type)
            {
                case RegType::INTEGER:
                    return READ_INT_REG<T>(state, reg_id.reg_num);
                    break;
                case RegType::FLOATING_POINT:
                    return READ_FP_REG<T>(state, reg_id.reg_num);
                    break;
                case RegType::CSR:
                    return READ_CSR_REG<T>(state, reg_id.reg_num);
                    break;
                default:
                    sparta_assert(false, "Invalid register type!");
            }
        }

        // std::vector<uint8_t> makeVectorRegValue(const std::vector<uint64_t> & words);

        std::vector<uint64_t> readVectorRegister_(PegasusState* state, const RegId & reg_id) const;

        std::string formatVectorHex_(const std::vector<uint64_t> & vec) const;

      private:
        sparta::utils::ValidValue<ObserverMode> arch_;

        void inspectInitialState_(PegasusState* state);

        virtual void preExecute_(PegasusState*) {}

        virtual void postExecute_(PegasusState*) {}

        virtual void preException_(PegasusState*) {}

        void reset_()
        {
            pc_ = 0;
            priv_mode_ = PrivMode::INVALID;
            virtual_mode_ = false;
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

        std::vector<uint8_t> readRegister_(const sparta::Register* reg) const
        {
            const size_t num_bytes = reg->getNumBytes();
            std::vector<uint8_t> value(num_bytes, 0);
            const uint32_t offset = 0;
            reg->peek(value.data(), num_bytes, offset);
            return value;
        }

        // Callbacks
        void postCsrWrite_(const sparta::TreeNode &, const sparta::TreeNode &,
                           const sparta::Register::PostWriteAccess &);
        void postCsrRead_(const sparta::TreeNode &, const sparta::TreeNode &,
                          const sparta::Register::ReadAccess &);
        void postMemWrite_(const sparta::memory::BlockingMemoryIFNode::PostWriteAccess &);
        void postMemRead_(const sparta::memory::BlockingMemoryIFNode::ReadAccess &);
    };

    std::ostream & operator<<(std::ostream & os, const Observer::ObservedValue & value);

} // namespace pegasus
