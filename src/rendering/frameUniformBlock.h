#pragma once
#include <core\globals.h>

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