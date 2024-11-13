#pragma once

#include "sparta/utils/SpartaAssert.hpp"

#include <vector>
#include <cstring> // for memcpy

namespace atlas
{
    template<typename T>
    inline std::vector<uint8_t> convertToByteVector(const T& value)
    {
        std::vector<uint8_t> byte_vector(sizeof(T));
        std::memcpy(byte_vector.data(), &value, sizeof(T));
        return byte_vector;
    }

    template<typename T>
    inline T convertFromByteVector(const std::vector<uint8_t>& byte_vector)
    {
        T value;
        sparta_assert(byte_vector.size() == sizeof(T),
            "Byte vector size does not match the size of the type");
        std::memcpy(&value, byte_vector.data(), sizeof(T));
        return value;
    }

    template<typename T1, typename T2>
    inline T2 signExtend(const T1 value)
    {
        static_assert(sizeof(T2) > sizeof(T1), "T2 must be larger than T1");
        constexpr uint32_t num_bits = sizeof(T1) * 8;
        T2 mask = 1 << (num_bits - 1);
        return (value ^ mask) - mask;
    }
}
