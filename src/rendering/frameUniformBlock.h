#pragma once
#include <phi.h>

namespace phi
{
    struct frameUniformBlock
    {
        mat4 p;
        mat4 v;
        mat4 vp;
        mat4 ip;
        vec2 resolution;
        float near;
        float far;
        float halfFovTangent;
        float time; 
        float pad0;
        float pad1;
    };
}