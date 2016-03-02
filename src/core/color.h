#pragma once

#include "core.h"

#include <algorithm>

namespace phi
{
    struct color
    {
    public:
        float r;
        float g;
        float b;
        float a;

    public:
        CORE_API color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

    public:
        CORE_API static color fromRGBA(long rgba)
        {
            int rgbaInt = (int) rgba;

            unsigned char r = rgbaInt & 255;
            rgbaInt = rgbaInt >> 8;
            unsigned  g = rgbaInt & 255;
            rgbaInt = rgbaInt >> 8;
            unsigned  b = rgbaInt & 255;
            rgbaInt = rgbaInt >> 8;
            unsigned  a = rgbaInt & 255;

            auto rf = std::max<float>(std::min<float>(std::abs((float) r / 255.0f), 1.0f), 0.0f);
            auto gf = std::max<float>(std::min<float>(std::abs((float) g / 255.0f), 1.0f), 0.0f);
            auto bf = std::max<float>(std::min<float>(std::abs((float) b / 255.0f), 1.0f), 0.0f);
            auto af = std::max<float>(std::min<float>(std::abs((float) a / 255.0f), 1.0f), 0.0f);

            return color(rf, gf, bf, af);
        }

        CORE_API static color fromRGBA(float r, float g, float b, float a)
        {
            return color(r, g, b, a);
        }

        CORE_API friend bool operator==(const color& a, const color& b)
        {
            return
                a.r == b.r &&
                a.g == b.g &&
                a.b == b.g &&
                a.a == b.a;
        }

        CORE_API friend bool operator!=(const color& a, const color& b)
        {
            return !(a == b);
        }

        CORE_API static color transparent;
        CORE_API static color black;
        CORE_API static color gray;
        CORE_API static color white;
        CORE_API static color red;
        CORE_API static color green;
        CORE_API static color blue;
        CORE_API static color yellow;
        CORE_API static color magenta;
        CORE_API static color cyan;
        CORE_API static color orange;
    };
}
