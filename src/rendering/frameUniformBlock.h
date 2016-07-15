#pragma once
#include <phi.h>

namespace phi
{
    struct frameUniformBlock
    {
        frameUniformBlock()
        {
        }

        frameUniformBlock(mat4 p, mat4 v) :
            p(p),
            v(v),
            vp(v * p),
            ip(glm::inverse(p))
        {
        }

        mat4 p;
        mat4 v;
        mat4 vp;
        mat4 ip;
    };
}