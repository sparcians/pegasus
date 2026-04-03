namespace pegasus
{
    // Functor for carry-less multiplication
    template <typename T> struct CarrylessMultiply
    {
        T operator()(T src1, T src2) const
        {
            T result = 0;

            // Perform carry-less multiplication
            for (size_t i = 0; i < 8 * sizeof(T); ++i)
            {
                if (src2 & (1ULL << i))
                {
                    result ^= (src1 << i);
                }
            }

            return result;
        }
    };

    // Functor for carry-less multiplication high part
    template <typename T> struct CarrylessMultiplyHigh
    {
        T operator()(T src1, T src2) const
        {
            T result = 0;
            for (size_t i = 1; i < 8 * sizeof(T); ++i)
            {
                if (src2 & (1ULL << i))
                {
                    result ^= (src1 >> (8 * sizeof(T) - i));
                }
            }
            return result;
        }
    };
} // namespace pegasus