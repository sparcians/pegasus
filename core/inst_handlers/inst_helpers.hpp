#pragma once

#include <stdint.h>

typedef int64_t sreg_t;
typedef uint64_t reg_t;
#define xlen 64

template <typename T>
inline sreg_t sext32(T x) {
    return (sreg_t)(int32_t)(x);
}

template <typename T>
inline reg_t zext32(T x) {
    return (reg_t)(uint32_t)(x);
}

template <typename T>
inline sreg_t sext(T x, uint32_t pos) {
    return (((sreg_t)(x) << (64 - (pos))) >> (64 - (pos)));
}

template <typename T>
inline reg_t zext(T x, uint32_t pos) {
    return (((reg_t)(x) << (64 - (pos))) >> (64 - (pos)));
}

template <typename T>
inline sreg_t sext_xlen(T x) {
    return sext(x, xlen);
}

template <typename T>
inline reg_t zext_xlen(T x) {
    return zext(x, xlen);
}

inline uint64_t mulhu(uint64_t a, uint64_t b)
{
    uint64_t t;
    uint32_t y1, y2, y3;
    uint64_t a0 = (uint32_t)a, a1 = a >> 32;
    uint64_t b0 = (uint32_t)b, b1 = b >> 32;

    t = a1*b0 + ((a0*b0) >> 32);
    y1 = t;
    y2 = t >> 32;

    t = a0*b1 + y1;

    t = a1*b1 + y2 + (t >> 32);
    y2 = t;
    y3 = t >> 32;

    return ((uint64_t)y3 << 32) | y2;
}

inline int64_t mulhsu(int64_t a, uint64_t b)
{
    int negate = a < 0;
    uint64_t res = mulhu(a < 0 ? -a : a, b);
    return negate ? ~res + (a * b == 0) : res;
}

inline int64_t mulh(int64_t a, int64_t b)
{
    int negate = (a < 0) != (b < 0);
    uint64_t res = mulhu(a < 0 ? -a : a, b < 0 ? -b : b);
    return negate ? ~res + ((uint64_t)a * (uint64_t)b == 0) : res;
}
