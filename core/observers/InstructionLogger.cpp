#include "core/observers/InstructionLogger.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "include/AtlasUtils.hpp"

#include "system/AtlasSystem.hpp"

#include "sparta/utils/LogUtils.hpp"
#include "sparta/log/MessageSource.hpp"

namespace atlas
{
#ifndef INSTLOG
#define INSTLOG(msg) SPARTA_LOG(inst_logger_, msg)
#endif

    class InstLogWriterBase
    {
      public:
        InstLogWriterBase(sparta::log::MessageSource & inst_logger) : inst_logger_(inst_logger) {}

        virtual ~InstLogWriterBase() = default;

        virtual void beginInst(AtlasState* state, const AtlasInst* inst, uint64_t opcode)
        {
            (void)state;
            (void)inst;
            (void)opcode;
        }

        virtual void writeSymbols(const std::string & symbols) { (void)symbols; }

        virtual void writeInstHeader(const AtlasInst* inst, uint64_t pc, uint64_t opcode)
        {
            (void)inst;
            (void)pc;
            (void)opcode;
        }

        virtual void writeFaultCause(const FaultCause cause) { (void)cause; }

        virtual void writeImmediate(const uint64_t imm) { (void)imm; }

        virtual void writeSrcRegister(const std::string & reg_name, const uint64_t reg_value,
                                      const uint32_t reg_width)
        {
            (void)reg_name;
            (void)reg_value;
            (void)reg_width;
        }

        virtual void writeDstRegister(const std::string & reg_name, const uint64_t reg_value,
                                      const uint64_t reg_prev_value, const uint32_t reg_width)
        {
            (void)reg_name;
            (void)reg_value;
            (void)reg_prev_value;
            (void)reg_width;
        }

        virtual void writeDstCSR(const std::string & reg_name, const uint32_t reg_num,
                                 const uint64_t reg_value, const uint64_t reg_prev_value,
                                 const uint32_t reg_width)
        {
            (void)reg_num;
            writeDstRegister(reg_name, reg_value, reg_prev_value, reg_width);
        }

        virtual void writeMemRead(const Observer::MemRead & mem_read) { (void)mem_read; }

        virtual void writeMemWrite(const Observer::MemWrite & mem_write) { (void)mem_write; }

        virtual void finishInst() {}

      protected:
        sparta::log::MessageSource & inst_logger_;
    };

    template <typename XLEN> class AtlasInstLogWriter : public InstLogWriterBase
    {
      public:
        static constexpr uint32_t WIDTH = std::is_same_v<XLEN, RV64> ? 16 : 8;

        AtlasInstLogWriter(sparta::log::MessageSource & inst_logger) :
            InstLogWriterBase(inst_logger)
        {
        }

        void writeSymbols(const std::string & symbols) override
        {
            reset_();
            inst_oss_ << "Call <" << symbols << ">";
            postExecute_(inst_oss_.str());
        }

        void writeInstHeader(const AtlasInst* inst, uint64_t pc, uint64_t opcode) override
        {
            reset_();
            if (inst)
            {
                inst_oss_ << HEX(pc, WIDTH) << ": " << inst->dasmString() << " (" << HEX8(opcode)
                          << ") uid:" << inst->getUid();
            }
            else
            {
                // TODO: Only display opcode for certain exception types
                inst_oss_ << HEX(pc, WIDTH) << ": ??? (" << HEX8(opcode) << ") uid: ?";
            }
            postExecute_(inst_oss_.str());
        }

        void writeFaultCause(const FaultCause cause) override
        {
            reset_();
            inst_oss_ << "Fault cause: " << cause << " (" << HEX8(static_cast<uint32_t>(cause))
                      << ")";
            postExecute_(inst_oss_.str());
        }

        void writeImmediate(const uint64_t imm) override
        {
            reset_();
            using SXLEN = typename std::make_signed<XLEN>::type;
            const SXLEN imm_val = imm;
            inst_oss_ << "   imm: " << HEX(imm_val, WIDTH);
            postExecute_(inst_oss_.str());
        }

        void writeSrcRegister(const std::string & reg_name, const uint64_t reg_value,
                              const uint32_t reg_width) override
        {
            reset_();
            inst_oss_ << "   src " << std::setfill(' ') << std::setw(3) << reg_name << ": "
                      << HEX(reg_value, reg_width);
            postExecute_(inst_oss_.str());
        }

        void writeDstRegister(const std::string & reg_name, const uint64_t reg_value,
                              const uint64_t reg_prev_value, const uint32_t reg_width) override
        {
            reset_();
            inst_oss_ << "   dst " << std::setfill(' ') << std::setw(3) << reg_name << ": "
                      << HEX(reg_value, reg_width) << " (prev: " << HEX(reg_prev_value, reg_width)
                      << ")";
            postExecute_(inst_oss_.str());
        }

        void writeMemRead(const Observer::MemRead & mem_read) override
        {
            reset_();
            inst_oss_ << "   mem read:  addr: " << HEX(mem_read.addr, WIDTH)
                      << ", size: " << mem_read.size << ", value: " << HEX(mem_read.value, WIDTH);
            postExecute_(inst_oss_.str());
        }

        void writeMemWrite(const Observer::MemWrite & mem_write) override
        {
            reset_();
            inst_oss_ << "   mem write: addr: " << HEX(mem_write.addr, WIDTH)
                      << ", size: " << mem_write.size << ", value: " << HEX(mem_write.value, WIDTH)
                      << " (prev: " << HEX(mem_write.prior_value, WIDTH) << ")";
            postExecute_(inst_oss_.str());
        }

        void finishInst() override { postExecute_(""); }

      private:
        void reset_()
        {
            inst_oss_.str("");
            inst_oss_.clear();
        }

        void postExecute_(const std::string & msg)
        {
            // The reason we call INSTLOG inside postExecute_() is because
            // we want the log to look like this:
            //
            //     postExecute_: 0x8000000c: li x1, +0xf (0xdeadbeef) uid: 34
            //     postExecute_:    imm: 0xf
            //     postExecute_:    dst x1: 0xf (prev: 0x0)
            //
            // Instead of this:
            //
            //     writeInstHeader:  0x8000000c: li x1, +0xf (0xdeadbeef) uid: 34
            //     writeImmediate:      imm: 0xf
            //     writeDstRegister:    dst x1: 0xf (prev: 0x0)
            //
            // We also use the somewhat confusing method name postExecute_() here since
            // that is the InstructionLogger's calling function name.

            INSTLOG(msg);
        }

        std::ostringstream inst_oss_;
    };

    template <typename XLEN> class SpikeInstLogWriter : public InstLogWriterBase
    {
      public:
        static constexpr uint32_t WIDTH = std::is_same_v<XLEN, RV64> ? 16 : 8;

        SpikeInstLogWriter(sparta::log::MessageSource & inst_logger) :
            InstLogWriterBase(inst_logger)
        {
        }

        void beginInst(AtlasState* state, const AtlasInst*, uint64_t opcode) override
        {
            reset_();
            inst_oss_ << "core   " << state->getHartId() << ": " << (int)state->getPrivMode() << " "
                      << HEX(state->getPc(), WIDTH) << " (" << HEX8(opcode) << ")";
        }

        void writeDstRegister(const std::string & reg_name, const uint64_t reg_value,
                              const uint64_t reg_prev_value, const uint32_t reg_width) override
        {
            (void)reg_prev_value;
            inst_oss_ << " " << std::setw(4) << std::left << reg_name << HEX(reg_value, reg_width);
        }

        void writeDstCSR(const std::string & reg_name, const uint32_t reg_num,
                         const uint64_t reg_value, const uint64_t reg_prev_value,
                         const uint32_t reg_width) override
        {
            (void)reg_prev_value;
            inst_oss_ << " c" << reg_num << "_" << reg_name << " " << HEX(reg_value, reg_width);
        }

        void writeMemRead(const Observer::MemRead & mem_read) override
        {
            inst_oss_ << " mem " << HEX(mem_read.addr, WIDTH);
        }

        void writeMemWrite(const Observer::MemWrite & mem_write) override
        {
            inst_oss_ << " mem " << HEX(mem_write.addr, WIDTH) << " " << HEX8(mem_write.value);
        }

        void finishInst() override { postExecute_(inst_oss_.str()); }

      private:
        void reset_()
        {
            inst_oss_.str("");
            inst_oss_.clear();
        }

        void postExecute_(const std::string & msg)
        {
            // clang-format off

            // The reason we call INSTLOG inside postExecute_() is because
            // we want the log to look like this:
            //
            //     postExecute_: core   0: 3 0x800000e4 (0x30529073) x0  0x00000000 c773_mtvec 0x800000e8
            //
            // Instead of this:
            //
            //     beginInst:         core   0: 3 0x800000e4 (0x30529073)
            //     writeDstRegister:  x0  0x00000000
            //     writeDstCSR:       c773_mtvec 0x800000e8
            //
            // We also use the somewhat confusing method name postExecute_() here since
            // that is the InstructionLogger's calling function name.

            // clang-format on
            INSTLOG(msg);
        }

        std::ostringstream inst_oss_;
    };

    template <typename XLEN>
    InstructionLogger<XLEN>::InstructionLogger(sparta::log::MessageSource & inst_logger) :
        inst_logger_(inst_logger),
        inst_log_writer_(std::make_shared<AtlasInstLogWriter<XLEN>>(inst_logger_))
    {
    }

    template <typename XLEN> void InstructionLogger<XLEN>::useSpikeFormatting()
    {
        inst_log_writer_ = std::make_shared<SpikeInstLogWriter<XLEN>>(inst_logger_);
    }

    template class InstructionLogger<RV64>;
    template class InstructionLogger<RV32>;

    template <typename XLEN> ActionGroup* InstructionLogger<XLEN>::preExecute_(AtlasState* state)
    {
        pc_ = state->getPc();
        AtlasInstPtr inst = state->getCurrentInst();
        if (inst)
        {
            opcode_ = inst->getOpcode();

            // Get value of source registers
            if (inst->hasRs1())
            {
                const auto rs1_reg = inst->getRs1Reg();
                const std::vector<uint8_t> value = getRegByteVector_(rs1_reg);
                src_regs_.emplace_back(getRegId(rs1_reg), value);
            }

            if (inst->hasRs2())
            {
                const auto rs2_reg = inst->getRs2Reg();
                const std::vector<uint8_t> value = getRegByteVector_(rs2_reg);
                src_regs_.emplace_back(getRegId(rs2_reg), value);
            }

            // Get initial value of destination registers
            if (inst->hasRd())
            {
                const auto rd_reg = inst->getRdReg();
                const std::vector<uint8_t> value = getRegByteVector_(rd_reg);
                dst_regs_.emplace_back(getRegId(rd_reg), value);
            }

            // For instructions that read/write CSRs
            if (inst->hasCsr())
            {
                const auto csr_reg = state->getCsrRegister(inst->getCsr());
                if (csr_reg)
                {
                    const std::vector<uint8_t> value = getRegByteVector_(csr_reg);
                    dst_regs_.emplace_back(getRegId(csr_reg), value);
                }
            }
        }

        return nullptr;
    }

    template <typename XLEN> ActionGroup* InstructionLogger<XLEN>::preException_(AtlasState* state)
    {
        preExecute(state);

        // Get value of source registers
        fault_cause_ = state->getExceptionUnit()->getUnhandledFault();
        interrupt_cause_ = state->getExceptionUnit()->getUnhandledInterrupt();
        return nullptr;
    }

    template <typename XLEN> ActionGroup* InstructionLogger<XLEN>::postExecute_(AtlasState* state)
    {
        // Get final value of destination registers
        AtlasInstPtr inst = state->getCurrentInst();

        if (fault_cause_.isValid() == false)
        {
            sparta_assert(inst != nullptr, "Instruction is not valid for logging!");
        }

        if (inst)
        {
            for (auto & dst_reg : dst_regs_)
            {
                sparta::Register* reg = nullptr;
                switch (dst_reg.reg_id.reg_type)
                {
                    case RegType::INTEGER:
                        reg = state->getIntRegister(dst_reg.reg_id.reg_num);
                        break;
                    case RegType::FLOATING_POINT:
                        reg = state->getFpRegister(dst_reg.reg_id.reg_num);
                        break;
                    case RegType::VECTOR:
                        reg = state->getVecRegister(dst_reg.reg_id.reg_num);
                        break;
                    case RegType::CSR:
                        reg = state->getCsrRegister(dst_reg.reg_id.reg_num);
                        break;
                    default:
                        sparta_assert(false, "Invalid register type!");
                }
                sparta_assert(reg != nullptr);
                const std::vector<uint8_t> value = getRegByteVector_(reg);
                dst_reg.setValue(value);
            }
        }

        inst_log_writer_->beginInst(state, inst.get(), opcode_);

        // Write to instruction logger
        const auto & symbols = state->getAtlasSystem()->getSymbols();
        if (symbols.find(pc_) != symbols.end())
        {
            inst_log_writer_->writeSymbols(symbols.at(pc_));
        }

        inst_log_writer_->writeInstHeader(inst.get(), pc_, opcode_);

        if (fault_cause_.isValid())
        {
            inst_log_writer_->writeFaultCause(fault_cause_.getValue());
        }

        if (inst && inst->hasImmediate())
        {
            inst_log_writer_->writeImmediate(inst->getImmediate());
        }

        // TODO: Support for different register sizes (RV32, vector)
        for (const auto & src_reg : src_regs_)
        {
            const uint32_t reg_width = src_reg.reg_value.size() * 2;
            const uint64_t reg_value = getRegValue_(src_reg.reg_value);
            inst_log_writer_->writeSrcRegister(src_reg.reg_id.reg_name, reg_value, reg_width);
        }

        for (const auto & dst_reg : dst_regs_)
        {
            const uint32_t reg_width = dst_reg.reg_value.size() * 2;
            const uint64_t reg_value = getRegValue_(dst_reg.reg_value);
            const uint64_t reg_prev_value = getRegValue_(dst_reg.reg_prev_value);

            if (dst_reg.reg_id.reg_type == RegType::CSR)
            {
                inst_log_writer_->writeDstCSR(dst_reg.reg_id.reg_name, dst_reg.reg_id.reg_num,
                                              reg_value, reg_prev_value, reg_width);
            }
            else
            {
                inst_log_writer_->writeDstRegister(dst_reg.reg_id.reg_name, reg_value,
                                                   reg_prev_value, reg_width);
            }
        }

        for (const auto & mem_read : mem_reads_)
        {
            inst_log_writer_->writeMemRead(mem_read);
        }

        for (const auto & mem_write : mem_writes_)
        {
            inst_log_writer_->writeMemWrite(mem_write);
        }

        inst_log_writer_->finishInst();
        return nullptr;
    }
} // namespace atlas
