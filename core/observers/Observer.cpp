#include "core/observers/Observer.hpp"
#include "include/PegasusUtils.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "core/VectorConfig.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{
    void Observer::preExecute(PegasusState* state)
    {
        reset_();
        inspectInitialState_(state);

        // Subclass impl
        preExecute_(state);
    }

    void Observer::preException(PegasusState* state)
    {
        // We want to reuse the preExecute() code but we do not want to
        // call the subclass' preExecute_() method twice (once normally
        // and once here during exception handling).
        //
        // That is why we call reset_() and inspectInitialState_() here
        // instead of preExecute().
        reset_();
        inspectInitialState_(state);

        // Get value of source registers
        fault_cause_ = state->getExceptionUnit()->getUnhandledFault();
        interrupt_cause_ = state->getExceptionUnit()->getUnhandledInterrupt();

        // Subclass impl
        preException_(state);
    }

    void Observer::postExecute(PegasusState* state)
    {
        // Get final value of destination registers
        PegasusInstPtr inst = state->getCurrentInst();

        if (fault_cause_.isValid() == false)
        {
            sparta_assert(inst != nullptr, "Instruction is not valid for logging!");
        }

        if (inst && arch_.isValid())
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
                dst_reg.setNewValue(readRegister_(reg));
            }
        }

        // Subclass impl
        postExecute_(state);
    }

    void Observer::inspectInitialState_(PegasusState* state)
    {
        pc_ = state->getPc();
        priv_mode_ = state->getPrivMode();
        virtual_mode_ = state->getVirtualMode();
        const PegasusInstPtr & inst = state->getCurrentInst();

        if (inst)
        {
            opcode_ = inst->getOpcode();

            if (arch_.isValid())
            {
                // Get value of source registers
                for (auto & src_reg : inst->getMavisOpcodeInfo()->getSourceOpInfoList())
                {
                    const auto reg = state->getSpartaRegister(&src_reg);
                    const auto reg_id = getRegId(reg);

                    if (reg_id.reg_type != RegType::VECTOR)
                    {
                        // base register value
                        src_regs_.emplace_back(SrcReg(reg_id, readRegister_(reg)));
                    }
                    else
                    {
                        const uint32_t encoded_lmul = inst->getVectorConfig()->getLMUL();

                        // works well for fractional lmul cases
                        uint32_t reg_count = 1;
                        if (false == inst->isVectorMaskOp())
                        {
                            reg_count = std::max(1u, encoded_lmul / 8);
                        }
                        const uint32_t base = reg_id.reg_num;

                        sparta_assert(
                            base + reg_count <= 32,
                            "Somehow we're blowing past the total number of vector insts: "
                                << inst);

                        for (uint32_t i = 0; i < reg_count; ++i)
                        {
                            const uint32_t phys = base + i;
                            RegId reg_id_lmul_src(RegType::VECTOR, phys,
                                                  "V" + std::to_string(phys));
                            src_regs_.emplace_back(SrcReg(
                                reg_id_lmul_src, readVectorRegister_(state, reg_id_lmul_src)));
                        }
                    }
                }

                // Get value of destination registers
                for (auto & dst_reg : inst->getMavisOpcodeInfo()->getDestOpInfoList())
                {
                    const auto reg = state->getSpartaRegister(&dst_reg);
                    // Can't write to x0
                    if (((RegType)reg->getGroupNum() == RegType::INTEGER)
                        && (reg->getGroupIdx() == 0))
                    {
                        continue;
                    }

                    const auto reg_id = getRegId(reg);
                    if (reg_id.reg_type != RegType::VECTOR)
                    {
                        dst_regs_.emplace_back(reg_id, readRegister_(reg));
                    }
                    else
                    {

                        const uint32_t encoded_lmul = inst->getVectorConfig()->getLMUL();
                        // works well for fractional lmul cases
                        uint32_t reg_count = 1;
                        if (false == inst->isVectorMaskOp())
                        {
                            reg_count = std::max(1u, encoded_lmul / 8);
                        }

                        const uint32_t base = reg_id.reg_num;
                        sparta_assert(
                            base + reg_count <= 32,
                            "Somehow we're blowing past the total number of vector insts: "
                                << inst);

                        for (uint32_t i = 0; i < reg_count; ++i)
                        {
                            const uint32_t phys = base + i;
                            RegId reg_id_lmul_dst(RegType::VECTOR, phys,
                                                  "V" + std::to_string(phys));
                            dst_regs_.emplace_back(DestReg(
                                reg_id_lmul_dst, readVectorRegister_(state, reg_id_lmul_dst)));
                        }
                    }
                }
            }
        }
    }

    void Observer::postCsrWrite_(const sparta::TreeNode &, const sparta::TreeNode &,
                                 const sparta::Register::PostWriteAccess & data)
    {
        const auto csr_reg = data.reg;
        const auto csr_num = csr_reg->getID();
        const RegId reg_id{(RegType)csr_reg->getGroupNum(), csr_num, csr_reg->getName()};

        const uint64_t final_value = (csr_reg->getNumBits() == 64) ? data.final->read<uint64_t>()
                                                                   : data.final->read<uint32_t>();
        // If this CSR has already been written to, just update the final value
        if (csr_writes_.find(csr_num) != csr_writes_.end())
        {
            csr_writes_.at(csr_num).setNewValue(final_value);
        }
        else
        {
            const uint64_t prior_value = (csr_reg->getNumBits() == 64)
                                             ? data.prior->read<uint64_t>()
                                             : data.prior->read<uint32_t>();
            csr_writes_.insert({csr_num, DestReg(reg_id, prior_value, final_value)});
        }

        // No need to also capture a read if there is a write since the write records the previous
        // value
        csr_reads_.erase(csr_num);
    }

    void Observer::postCsrRead_(const sparta::TreeNode &, const sparta::TreeNode &,
                                const sparta::Register::ReadAccess & data)
    {
        const auto csr_reg = data.reg;
        const auto csr_num = csr_reg->getID();
        const RegId reg_id{(RegType)csr_reg->getGroupNum(), csr_num, csr_reg->getName()};
        if ((csr_reads_.find(csr_num) == csr_reads_.end())
            && (csr_writes_.find(csr_num) == csr_writes_.end()))
        {
            const uint64_t value = (csr_reg->getNumBits() == 64) ? data.value->read<uint64_t>()
                                                                 : data.value->read<uint32_t>();
            csr_reads_.insert({csr_num, SrcReg(reg_id, value)});
        }
    }

    void Observer::postMemWrite_(const sparta::memory::BlockingMemoryIFNode::PostWriteAccess & data)
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

        // Get vaddr and source
        const PegasusState::MemorySupplement* supplement =
            reinterpret_cast<const PegasusState::MemorySupplement*>(data.in_supplement);

        mem_writes_.emplace_back(supplement->paddr, supplement->vaddr, data.size, final_val,
                                 prior_val, supplement->source);
    }

    void Observer::postMemRead_(const sparta::memory::BlockingMemoryIFNode::ReadAccess & data)
    {
        uint64_t val = 0;
        for (size_t i = 0; i < data.size; ++i)
        {
            val |= static_cast<uint64_t>(data.data[i]) << (i * 8);
        }

        // Get vaddr and source
        const PegasusState::MemorySupplement* supplement =
            reinterpret_cast<const PegasusState::MemorySupplement*>(data.in_supplement);

        mem_reads_.emplace_back(supplement->paddr, supplement->vaddr, data.size, val,
                                supplement->source);
    }

    std::vector<uint64_t> Observer::readVectorRegister_(PegasusState* state,
                                                        const RegId & reg_id) const
    {
        const uint32_t vlen_bits = state->getVectorConfig()->getVLEN();
        switch (vlen_bits)
        {
            case 128:
                {
                    using VLEN = std::array<uint64_t, 2>;
                    const auto ary = READ_VEC_REG<VLEN>(state, reg_id.reg_num);
                    return std::vector<uint64_t>(ary.begin(), ary.end());
                }
            case 256:
                {
                    using VLEN = std::array<uint64_t, 4>;
                    const auto ary = READ_VEC_REG<VLEN>(state, reg_id.reg_num);
                    return std::vector<uint64_t>(ary.begin(), ary.end());
                }
            case 512:
                {
                    using VLEN = std::array<uint64_t, 8>;
                    const auto ary = READ_VEC_REG<VLEN>(state, reg_id.reg_num);
                    return std::vector<uint64_t>(ary.begin(), ary.end());
                }
            case 1024:
                {
                    using VLEN = std::array<uint64_t, 16>;
                    const auto ary = READ_VEC_REG<VLEN>(state, reg_id.reg_num);
                    return std::vector<uint64_t>(ary.begin(), ary.end());
                }
            default:
                sparta_assert(false, "Unsupported VLEN size: " + std::to_string(vlen_bits));
        }

        return std::vector<uint64_t>();
    }

    std::string Observer::formatVectorHex_(const std::vector<uint64_t> & vec) const
    {
        std::ostringstream oss;
        for (const auto & val : vec)
        {
            oss << std::hex << val;
        }
        return oss.str();
    }

    std::ostream & operator<<(std::ostream & os, const Observer::ObservedValue & value)
    {
        os << "0x" << sparta::utils::bin_to_hexstr(value.getByteVector().data(), value.size(), "");
        return os;
    }

} // namespace pegasus
