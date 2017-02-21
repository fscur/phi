#include <precompiled.h>
#include "string.h"

namespace phi
{
    string to_string(ivec2 value)
    {
        return "[" + std::to_string(value.x) + "; " + std::to_string(value.y) + "]";
    }

    string to_string(vec2 value)
    {
        return "[" + std::to_string(value.x) + "; " + std::to_string(value.y) + "]";
    }

    string to_string(vec3 value)
    {
        return "[" + std::to_string(value.x) + "; " + std::to_string(value.y) + "; " + std::to_string(value.z) + "]";
    }

    string to_string(vec4 value)
    {
        return "[" + std::to_string(value.x) + "; " + std::to_string(value.y) + "; " + std::to_string(value.z) + "; " + std::to_string(value.w) + "]";
    }
}