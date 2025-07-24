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

        const Mode dst, src1, src2;

        constexpr OperandMode(Mode dst, Mode src2, Mode src1) : dst(dst), src1(src1), src2(src2) {}
    };
} // namespace pegasus