#pragma once
#include <phi.h>

namespace phi
{
    struct scissor
    {
    public:
        float x;
        float y;
        float w;
        float h;

        scissor::scissor();
        scissor(float x, float y, float w, float h);
        scissor intersection(scissor b);
        bool intersectsWith(vec2 point);
    };
}