#pragma once

namespace pegasus
{
    struct OperandMode
    {
        enum struct Mode
        {
            V, // vector
            X, // scalar int
            F, // scalar float
            S, // single vector element[0]
            I, // immediate
            W, // wide
            N  // not used
        };

        const Mode dst = Mode::N;
        const Mode src2 = Mode::N;
        const Mode src1 = Mode::N;
    };
} // namespace pegasus