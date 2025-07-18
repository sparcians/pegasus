#pragma once

#include "sparta/utils/SpartaAssert.hpp"
#include <type_traits>

#include <vector>
#include <cstring> // for memcpy

namespace pegasus
{
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

    template <typename T> constexpr auto get_uint_type()
    {
        if constexpr (std::is_same_v<T, int8_t>)
            return std::type_identity<uint8_t>{};
        else if constexpr (std::is_same_v<T, int16_t>)
            return std::type_identity<uint16_t>{};
        else if constexpr (std::is_same_v<T, int32_t>)
            return std::type_identity<uint32_t>{};
        else if constexpr (std::is_same_v<T, int64_t>)
            return std::type_identity<uint64_t>{};
        else
            static_assert(std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t>
                              || std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>,
                          "Unsupported unsigned type");
    }

    template <typename T> using UnsignedType = typename decltype(get_uint_type<T>())::type;

    template <typename T> constexpr auto get_int_type()
    {
        if constexpr (std::is_same_v<T, uint8_t>)
            return std::type_identity<int8_t>{};
        else if constexpr (std::is_same_v<T, uint16_t>)
            return std::type_identity<int16_t>{};
        else if constexpr (std::is_same_v<T, uint32_t>)
            return std::type_identity<int32_t>{};
        else if constexpr (std::is_same_v<T, uint64_t>)
            return std::type_identity<int64_t>{};
        else
            static_assert(std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t>
                              || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>,
                          "Unsupported unsigned type");
    }

    template <typename T> using SignedType = typename decltype(get_int_type<T>())::type;
} // namespace atlas
