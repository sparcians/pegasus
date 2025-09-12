#include "STFValidator.hpp"
#include "core/PegasusState.hpp"
#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{
#ifndef STFVALIDLOG
#define STFVALIDLOG(msg) SPARTA_LOG(stf_valid_logger_, msg)
#endif

    STFValidator::STFValidator(sparta::log::MessageSource & stf_valid_logger,
                               const ObserverMode arch, const uint64_t initial_pc,
                               const std::string & stf_filename) :
        Observer(arch),
        stf_valid_logger_(stf_valid_logger)
    {
        std::ifstream fs;
        std::ios_base::iostate exceptionMask = fs.exceptions() | std::ios::failbit;
        fs.exceptions(exceptionMask);

        try
        {
            fs.open(stf_filename);
        }
        catch (const std::ifstream::failure & e)
        {
            throw sparta::SpartaException("ERROR: Issues opening ")
                << stf_filename << ": " << e.what();
        }

        stf_reader_.reset(new stf::STFInstReader(stf_filename));
        next_it_ = stf_reader_->begin();

        uint64_t stf_pc = stf_reader_->getInitialPC();
        if (initial_pc != stf_pc)
        {
            // Traces from Spike have 5 instructions of boot code before entering the actual
            // workload
            uint64_t skip_count = 0;
            while (skip_count < 5)
            {
                ++next_it_;
                stf_pc = next_it_->pc();
                if (initial_pc == stf_pc)
                {
                    break;
                }
                ++skip_count;
            }
            sparta_assert(initial_pc == stf_pc, "Failed to find initial PC in validation trace!");
        }
    }

    void STFValidator::postExecute_(PegasusState* state)
    {
        if (next_it_ == stf_reader_->end())
        {
            return;
        }

        const uint64_t pc = state->getPrevPc();
        const uint64_t stf_pc = next_it_->pc();

        if (pc != stf_pc)
        {
            STFVALIDLOG("PCs have diverged!");
            STFVALIDLOG(state->getCurrentInst());
            STFVALIDLOG("    Pegasus PC: 0x" << std::hex << pc);
            STFVALIDLOG("        STF PC: 0x" << std::hex << stf_pc);
            STFVALIDLOG("Pegasus Opcode: 0x" << state->getSimState()->current_opcode)
            STFVALIDLOG("    STF Opcode: 0x" << next_it_->opcode());
            STFVALIDLOG("");
            sparta_assert(false, "PCs have diverged!");
        }

        auto get_stf_reg_type = [](const stf::Operand & stf_reg)
        {
            if (stf_reg.isInt())
            {
                return RegType::INTEGER;
            }
            else if (stf_reg.isFP())
            {
                return RegType::FLOATING_POINT;
            }
            else if (stf_reg.isVector())
            {
                return RegType::VECTOR;
            }
            else
            {
                return RegType::CSR;
            }
        };

        for (const auto & dst_reg : dst_regs_)
        {
            const RegType reg_type = dst_reg.reg_id.reg_type;
            const uint32_t reg_num = dst_reg.reg_id.reg_num;
            for (const auto & stf_dst_reg : next_it_->getDestOperands())
            {
                const RegType stf_reg_type = get_stf_reg_type(stf_dst_reg);
                const uint32_t stf_reg_num = stf::Registers::getArchRegIndex(stf_dst_reg.getReg());
                if ((reg_type == stf_reg_type) && (reg_num == stf_reg_num))
                {
                    if (reg_type != RegType::VECTOR)
                    {
                        uint64_t reg_val = readScalarRegister_<uint64_t>(state, dst_reg.reg_id);
                        uint64_t stf_reg_val = stf_dst_reg.getScalarValue();
                        if (reg_val != stf_reg_val)
                        {
                            STFVALIDLOG("Register writes do not match for dst "
                                        << std::dec << dst_reg.reg_id.reg_name);
                            STFVALIDLOG(state->getCurrentInst());
                            STFVALIDLOG("    Pegasus value: 0x" << std::hex << reg_val);
                            STFVALIDLOG("        STF value: 0x" << std::hex << stf_reg_val);
                            STFVALIDLOG("");
                        }
                    }
                    break;
                }
            }
        }

        ++next_it_;
    }
} // namespace pegasus
