#pragma once
#include <phi.h>

namespace phi
{
    struct resolution
    {
        struct resolution(float width = 0.0f, float height = 0.0f, vec2 dpi = vec2(0.0f)) :
            width(width),
            height(height),
            dpi(dpi)
        {
        }

        float getAspect() const { return width / height; }
        vec2 toVec2() const { return vec2(width, height); }

        string toString() const
        { 
            return "resolution (" + std::to_string(width) + "; " + std::to_string(height) + ")";
        }

        float width;
        float height;
        vec2 dpi;
    };
}