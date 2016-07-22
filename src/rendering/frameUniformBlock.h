#pragma once
#include <phi.h>

namespace phi
{
    struct frameUniformBlock
    {
        frameUniformBlock()
        {
        }

        frameUniformBlock(
            const mat4& p, 
            const mat4& v, 
            const resolution& resolution,
            const float time) :
            p(p),
            v(v),
            vp(v * p),
            ip(glm::inverse(p)),
            resolution(resolution.toVec2()),
            time(time),
            pad0(0.0f)
        {
        }

        mat4 p;
        mat4 v;
        mat4 vp;
        mat4 ip;
        vec2 resolution;
        float time; 
        float pad0;
    };
}