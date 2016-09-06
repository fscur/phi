#pragma once
#include <phi.h>
#include "coreApi.h"

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
    private:
        static float hueToRGB(float v1, float v2, float vH)
        {
            if (vH < 0.0f)
                vH += 1.0f;

            if (vH > 1.0f)
                vH -= 1.0f;

            if ((6.0f * vH) < 1.0f)
                return (v1 + (v2 - v1) * 6.0f * vH);

            if ((2.0f * vH) < 1.0f)
                return v2;

            if ((3.0f * vH) < 2.0f)
                return (v1 + (v2 - v1) * ((2.0f / 3.0f) - vH) * 6.0f);

            return v1;
        }

    public:
        static color fromHSL(float huehuebrbr, float saturation, float luminosity)
        {
            float r;
            float g;
            float b;

            if (saturation == 0.0f)
            {
                r = g = b = luminosity;
            }
            else
            {
                float v1;
                float v2;

                v2 = (luminosity < 0.5f) ? (luminosity * (1.0f + saturation)) : ((luminosity + saturation) - (luminosity * saturation));
                v1 = 2.0f * luminosity - v2;

                r = hueToRGB(v1, v2, huehuebrbr + (1.0f / 3.0f));
                g = hueToRGB(v1, v2, huehuebrbr);
                b = hueToRGB(v1, v2, huehuebrbr - (1.0f / 3.0f));
            }

            return color(r, g, b, 1.0f);
        }

        static color fromRGBA(long rgba)
        {
            int rgbaInt = (int)rgba;

            unsigned char r = rgbaInt & 255;
            rgbaInt = rgbaInt >> 8;
            unsigned  g = rgbaInt & 255;
            rgbaInt = rgbaInt >> 8;
            unsigned  b = rgbaInt & 255;
            rgbaInt = rgbaInt >> 8;
            unsigned  a = rgbaInt & 255;

            auto rf = std::max<float>(std::min<float>(std::abs((float)r / 255.0f), 1.0f), 0.0f);
            auto gf = std::max<float>(std::min<float>(std::abs((float)g / 255.0f), 1.0f), 0.0f);
            auto bf = std::max<float>(std::min<float>(std::abs((float)b / 255.0f), 1.0f), 0.0f);
            auto af = std::max<float>(std::min<float>(std::abs((float)a / 255.0f), 1.0f), 0.0f);

            return color(rf, gf, bf, af);
        }

        static color fromRGBA(float r, float g, float b, float a)
        {
            return color(r, g, b, a);
        }

        color color::operator+(const color& other)
        {
            return color(
                r + other.r,
                g + other.g,
                b + other.b,
                a + other.a);
        }

        color color::operator-(const color& other)
        {
            return color(
                r - other.r,
                g - other.g,
                b - other.b,
                a - other.a);
        }

        color color::operator*(float factor)
        {
            return color(
                r * factor,
                g * factor,
                b * factor,
                a * factor);
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
