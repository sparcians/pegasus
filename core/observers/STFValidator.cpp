#include "STFValidator.hpp"
#include "core/PegasusState.hpp"

namespace pegasus
{
    STFValidator::STFValidator(const uint64_t xlen, const uint64_t initial_pc,
                               const std::string & filename) :
        Observer((xlen == 32) ? ObserverMode::RV32 : ObserverMode::RV64)
    {
        std::ifstream fs;
        std::ios_base::iostate exceptionMask = fs.exceptions() | std::ios::failbit;
        fs.exceptions(exceptionMask);

        try
        {
            fs.open(filename);
        }
        catch (const std::ifstream::failure & e)
        {
            throw sparta::SpartaException("ERROR: Issues opening ") << filename << ": " << e.what();
        }

        stf_reader_.reset(new stf::STFInstReader(filename));
        next_it_ = stf_reader_->begin();

        uint64_t stf_pc = stf_reader_->getInitialPC();
        if (initial_pc != stf_pc)
        {
            uint64_t skip_count = 0;
            while (skip_count < 10)
            {
                ++next_it_;
                stf_pc = next_it_->pc();
                if (initial_pc == stf_pc)
                {
                    break;
                }
                ++skip_count;
            }
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
            std::cout << std::hex << "PC: 0x" << pc << ", STF PC: 0x" << stf_pc << std::endl;
            std::cout << state->getCurrentInst() << std::endl;
            sparta_assert(false, "PCs have diverged!");
        }
        std::cout << "0x" << std::hex << next_it_->opcode() << std::endl;
        ++next_it_;
    }
} // namespace pegasus
