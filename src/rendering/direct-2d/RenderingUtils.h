#pragma once

#include <d2d1.h>
#include <cstdint>
#include <algorithm>

namespace RenderingUtils {
    inline uint32_t packRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return (static_cast<uint32_t>(a) << 24) |
               (static_cast<uint32_t>(r) << 16) |
               (static_cast<uint32_t>(g) << 8)  |
               (static_cast<uint32_t>(b));
    }

    inline void unpackRGBA(uint32_t argb, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) {
        a = static_cast<uint8_t>((argb >> 24) & 0xFF);
        r = static_cast<uint8_t>((argb >> 16) & 0xFF);
        g = static_cast<uint8_t>((argb >> 8) & 0xFF);
        b = static_cast<uint8_t>(argb & 0xFF);
    }

    inline D2D1_COLOR_F colorF(float r, float g, float b, float a = 1.0f) {
        return D2D1::ColorF(r, g, b, a);
    }

    inline float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    template<typename T>
    inline T clamp(T val, T minVal, T maxVal) {
        return std::max(minVal, std::min(maxVal, val));
    }
}
