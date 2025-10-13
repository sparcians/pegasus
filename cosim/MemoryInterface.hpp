#pragma once

#include "include/PegasusTypes.hpp"
#include <vector>

namespace pegasus::cosim
{
    /**
     * \class CoSim
     *
     *
     */
    class MemoryInterface
    {
      public:
        //! Allow derivation
        virtual ~MemoryInterface() {}

        virtual bool peek(CoreId core_id, HartId hart_id, Addr paddr, size_t size,
                          std::vector<uint8_t> & buffer) const = 0;
        virtual bool read(CoreId core_id, HartId hart_id, Addr paddr, size_t size,
                          std::vector<uint8_t> & buffer) const = 0;
        virtual bool poke(CoreId core_id, HartId hart_id, Addr paddr,
                          std::vector<uint8_t> & buffer) const = 0;
        virtual bool write(CoreId core_id, HartId hart_id, Addr paddr,
                           std::vector<uint8_t> & buffer) const = 0;
    };
} // namespace pegasus::cosim
