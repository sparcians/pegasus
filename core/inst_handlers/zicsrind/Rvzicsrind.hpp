#pragma once

#include "core/PegasusState.hpp"
#include "core/inst_handlers/zicsrind/RvzicsrConstants.hpp"

namespace pegasus
{
    class Rvzicsrind
    {
        /*!
         * \brief Make sure the xiselect value is valid for the specified xiregN,
         * offset returns the index of the decoded xiselect register
         */
        template <typename XLEN, uint32_t XISELECT, uint32_t XIIDX>
        static CsrindRegType validate_xiselect(PegasusState* state, int & offset);

        // Custom register read/write callback functions
        template <typename XLEN, int XIIDX>
        static XLEN miregR(PegasusState* state, sparta::RegisterBase* reg);
        template <typename XLEN, int XIIDX>
        static void miregW(PegasusState* state, sparta::RegisterBase* reg, XLEN val);
        template <typename XLEN, int XIIDX>
        static XLEN siregR(PegasusState* state, sparta::RegisterBase* reg);
        template <typename XLEN, int XIIDX>
        static void siregW(PegasusState* state, sparta::RegisterBase* reg, XLEN val);

      public:
        /*!
         *  \brief Installs callback funcitons to xiselect/xiregN registers
         */
        template <typename XLEN> static void addCSRRegisterCallbacks(PegasusState* state);
    };
} // namespace pegasus
