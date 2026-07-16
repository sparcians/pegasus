#pragma once

#include "core/observers/Observer.hpp"
#include "stf_record_types.hpp"
#include "stf_writer.hpp"
#include "core/PegasusInst.hpp"

#include <optional>

namespace pegasus
{
    class STFLogger : public Observer
    {
      public:
        /*!
         * \class STFLogger
         * \brief Description...
         * \param reg_width Register width (32 or 64)
         * \param initial_pc Initial program counter
         * \param filename Name of the file the trace will be written to
         * \param state PegasusState used to populate initial register values
         * \param opcode_trigger The opcode to start a new STF trace file
         */
        STFLogger(const uint32_t reg_width, uint64_t initial_pc, const std::string & filename,
                  PegasusState* state, std::optional<uint32_t> opcode_trigger);

      private:
        stf::STFWriter stf_writer_;
        void postExecute_(PegasusState* state) override;

        template <typename XLEN> void recordRegState_(PegasusState* state);

        template <typename XLEN, typename F>
        void writeInstRegRecord_(PegasusState* state, F get_stf_reg_type);

        template <typename XLEN> void writeEventRecord_(PegasusState* state);

        void startSTFTrace_(uint64_t inital_pc, const std::string & filename, PegasusState* state);

        uint32_t trace_instance_ = 0;

        const std::string filename_;

        const std::optional<uint32_t> opcode_trigger_;

        const bool vector_enabled_;

        std::string current_symbol_;
    };
} // namespace pegasus
