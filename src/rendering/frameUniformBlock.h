#pragma once
#include <precompiled.h>

namespace phi
{
    struct frameUniformBlock
    {
        mat4 p;
        mat4 v;
        mat4 vp;
        mat4 ip;
    };
}