#pragma once

#include "core/observers/Observer.hpp"
#include "stf-inc/stf_inst_reader.hpp"

namespace pegasus
{
    class STFValidator : public Observer
    {
      public:
        STFValidator(sparta::log::MessageSource & stf_valid_logger, const ObserverMode arch,
                     const uint64_t initial_pc, const std::string & stf_filename);

      private:
        std::unique_ptr<stf::STFInstReader> stf_reader_;
        stf::STFInstReader::iterator next_it_;
        sparta::log::MessageSource & stf_valid_logger_;

        void postExecute_(PegasusState* state) override;
    };
} // namespace pegasus
