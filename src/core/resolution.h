#pragma once
#include <phi.h>

namespace phi
{
    struct resolution
    {
        resolution(float width = 0.0f, float height = 0.0f, vec2 dpi = vec2(0.0f)) :
            width(width),
            height(height),
            dpi(dpi)
        {
        }

        float getAspect() const { return width / height; }
        vec2 toVec2() const { return vec2(width, height); }

        string toString() const
        { 
            return "(" + std::to_string(width) + "; " + std::to_string(height) + ") [" + std::to_string(dpi.x) + "; " + std::to_string(dpi.y) + "]";
        }

        float width;
        float height;
        vec2 dpi;
    };
}