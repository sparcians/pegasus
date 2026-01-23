
#pragma once

#include "core/InstHandlers.hpp"

namespace pegasus {
class PegasusState;

class ZbkbInsts {
public:
    using base_type = ZbkbInsts;

    template <typename XLEN>
    static void getInstHandlers(InstHandlers::InstHandlersMap &);

private:
    // Logical
    template <typename XLEN>
    Action::ItrType andnHandler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType ornHandler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType xnorHandler_(PegasusState* state, Action::ItrType itr);

    // Rotations
    template <typename XLEN>
    Action::ItrType rolHandler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType rorHandler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType roriHandler_(PegasusState* state, Action::ItrType itr);

    // Byte/bit
    template <typename XLEN>
    Action::ItrType rev8Handler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType brev8Handler_(PegasusState* state, Action::ItrType itr);

    // Packing
    template <typename XLEN>
    Action::ItrType packHandler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType packhHandler_(PegasusState* state, Action::ItrType itr);

    // RV64-only
    template <typename XLEN>
    Action::ItrType packwHandler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType rolwHandler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType rorwHandler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType roriwHandler_(PegasusState* state, Action::ItrType itr);

    // RV32-only
    template <typename XLEN>
    Action::ItrType zipHandler_(PegasusState* state, Action::ItrType itr);

    template <typename XLEN>
    Action::ItrType unzipHandler_(PegasusState* state, Action::ItrType itr);

    // Helpers (can stay static inline)
    template <typename T>
    static inline T ror_(T value, unsigned int shift) {
        constexpr unsigned int bits = sizeof(T) * 8;
        shift &= (bits - 1);
        return (value >> shift) | (value << (bits - shift));
    }

    template <typename T>
    static inline T rol_(T value, unsigned int shift) {
        constexpr unsigned int bits = sizeof(T) * 8;
        shift &= (bits - 1);
        return (value << shift) | (value >> (bits - shift));
    }

    template <typename T>
    static inline T rev8_(T value) {
        if constexpr (sizeof(T) == 4) return __builtin_bswap32(value);
        if constexpr (sizeof(T) == 8) return __builtin_bswap64(value);
        return value;
    }

    template <typename T>
    static T brev8_(T value) {
        T result = 0;
        constexpr int num_bytes = sizeof(T);
        for (int i = 0; i < num_bytes; ++i) {
            uint8_t byte = (value >> (i * 8)) & 0xFF;
            uint8_t reversed = ((byte & 0x01) << 7) | ((byte & 0x02) << 5) |
                               ((byte & 0x04) << 3) | ((byte & 0x08) << 1) |
                               ((byte & 0x10) >> 1) | ((byte & 0x20) >> 3) |
                               ((byte & 0x40) >> 5) | ((byte & 0x80) >> 7);
            result |= (static_cast<T>(reversed) << (i * 8));
        }
        return result;
    }
};
}  // namespace pegasus