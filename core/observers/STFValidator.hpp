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
         * \brief Description...
         * \param reg_width Register width (32 or 64)
         * \param initial_pc Initial program counter
         * \param filename Name of the file the trace will be written to
         * \param state PegasusState used to populate initial register values
         */
        STFValidator(const uint64_t xlen, const uint64_t initial_pc, const std::string & filename);

      private:
        std::unique_ptr<stf::STFInstReader> stf_reader_;
        stf::STFInstReader::iterator next_it_;

        void postExecute_(PegasusState* state) override;
    };
} // namespace pegasus
