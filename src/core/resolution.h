#pragma once
#include <phi.h>

namespace phi
{
    struct resolution
    {
        struct resolution(float width, float height) :
            width(width),
            height(height)
        {
        }

        float getAspect() { return width / height; }
        vec2 toVec2() { return vec2(width, height); }
        
        string toString() 
        { 
            return "resolution (" + std::to_string(width) + "; " + std::to_string(height) + ")";
        }

        float width;
        float height;
    };
}