#pragma once
#include <phi.h>
#include "core.h"

namespace phi
{
    struct CORE_API color
    {
    public:
        float r;
        float g;
        float b;
        float a;

    public:
        color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

    public:
        static color fromRGBA(long rgba)
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

        static color fromRGBA(float r, float g, float b, float a)
        {
            return color(r, g, b, a);
        }

        friend bool operator==(const color& a, const color& b)
        {
            return
                a.r == b.r &&
                a.g == b.g &&
                a.b == b.g &&
                a.a == b.a;
        }

        friend bool operator!=(const color& a, const color& b)
        {
            return !(a == b);
        }

        static color transparent;
        static color black;
        static color gray;
        static color white;
        static color red;
        static color green;
        static color blue;
        static color yellow;
        static color magenta;
        static color cyan;
        static color orange;
    };
}
