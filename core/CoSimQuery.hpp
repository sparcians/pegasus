#pragma once

#include "include/AtlasTypes.hpp"

namespace atlas
{

// This base class is used to query the expected register values from the
// attached co-simulation e.g. Imperas, Spike, etc.
//
// It is used to optionally add expected input/output values to each instruction
// for Python implementations' quick evaluation in the Atlas IDE.
//
// Subclasses could be found external to the core Atlas codebase, such as
// a separate repo of yours that has a license to Imperas.
class CoSimQuery
{
public:
    virtual ~CoSimQuery() = default;
    virtual uint64_t getExpectedRegValue(RegType type, uint32_t regidx, uint64_t hart) const = 0;
    virtual uint64_t getExpectedPC(uint64_t hart) const = 0;
};

} // namespace atlas
