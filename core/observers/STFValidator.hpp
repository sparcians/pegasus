#pragma once

#include "core/observers/Observer.hpp"
#include "stf-inc/stf_inst_reader.hpp"

namespace pegasus
{
    class STFValidator : public Observer
    {
      public:
        STFValidator(sparta::log::MessageSource & stf_valid_logger, const ObserverMode arch,
                     const std::string & stf_filename, const uint64_t validate_trace_begin,
                     const uint64_t validate_inst_begin);

      private:
        std::unique_ptr<stf::STFInstReader> stf_reader_;
        stf::STFInstReader::iterator next_it_;
        sparta::log::MessageSource & stf_valid_logger_;
        const uint64_t validate_inst_begin_ = 0x1;

        void postExecute_(PegasusState* state) override;
    };
} // namespace pegasus
