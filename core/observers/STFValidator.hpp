#pragma once

#include "core/observers/Observer.hpp"
#include "stf-inc/stf_inst_reader.hpp"

namespace pegasus
{
    class STFValidator : public Observer
    {
      public:
        /*!
         * \class STFValidator
         * \brief Observer for validating Pegasus with an STF trace
         * \param xlen Either 32 or 64 bit
         * \param initial_pc Initial program counter
         * \param filename File name of the validation trace
         */
        STFValidator(const uint64_t xlen, const uint64_t initial_pc, const std::string & filename);

      private:
        std::unique_ptr<stf::STFInstReader> stf_reader_;
        stf::STFInstReader::iterator next_it_;

        void postExecute_(PegasusState* state) override;
    };
} // namespace pegasus
