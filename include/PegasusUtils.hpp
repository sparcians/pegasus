#pragma once

#include "sparta/utils/SpartaAssert.hpp"
#include <type_traits>

#include <vector>
#include <cstring> // for memcpy

namespace pegasus
{
    // Convert a byte vector any type as long as the size of the type is greater than or equal to
    // the size of the byte vector
    template <typename T> inline T getValueFromByteVector(const std::vector<uint8_t> & byte_vector)
    {
        sparta_assert(byte_vector.size() <= sizeof(T), "");
        T value = 0;
        for (size_t idx = 0; idx < byte_vector.size(); ++idx)
        {
            value |= (T)byte_vector[idx] << (idx * 8);
        }
        return value;
    }

    template <typename T> inline std::vector<uint8_t> convertToByteVector(const T & value)
    {
        std::vector<uint8_t> byte_vector(sizeof(T));
        std::memcpy(byte_vector.data(), &value, sizeof(T));
        return byte_vector;
    }

    template <typename T> inline T convertFromByteVector(const std::vector<uint8_t> & byte_vector)
    {
        T value;
        sparta_assert(byte_vector.size() == sizeof(T),
                      "Byte vector size does not match the size of the type");
        std::memcpy(&value, byte_vector.data(), sizeof(T));
        return value;
    }

    template <typename T1, typename T2> inline T2 signExtend(const T1 value)
    {
        static_assert(sizeof(T2) > sizeof(T1), "T2 must be larger than T1");
        constexpr uint32_t num_bits = sizeof(T1) * 8;
        T2 mask = 1 << (num_bits - 1);
        return (value ^ mask) - mask;
    }

    template <std::size_t N> inline constexpr auto get_uint_type()
    {
        if constexpr (N == 8)
            return std::type_identity<uint8_t>{};
        else if constexpr (N == 16)
            return std::type_identity<uint16_t>{};
        else if constexpr (N == 32)
            return std::type_identity<uint32_t>{};
        else if constexpr (N == 64)
            return std::type_identity<uint64_t>{};
        else
            static_assert(N == 8 || N == 16 || N == 32 || N == 64, "Unsupported size");
    }

    template <std::size_t N> using UintType = typename decltype(get_uint_type<N>())::type;

    template <typename T> struct FuncTraits;

    template <typename R, typename... Args> struct FuncTraits<R (*)(Args...)>
    {
        using ReturnType = R;
        using ArgsTuple = std::tuple<Args...>;
    };
} // namespace pegasus
