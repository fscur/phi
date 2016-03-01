#ifndef _PHI_SCISSOR_H_
#define _PHI_SCISSOR_H_

#include <core\globals.h>

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

#endif