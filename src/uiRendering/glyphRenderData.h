#pragma once
#include <phi.h>

namespace phi
{
    struct glyphRenderData
    {
        color color;
        vec2 position;
        vec2 size;
        float shift;
        float page;
        float pad0;
        float pad1;
        int unit;
        int pad2;
        int pad3;
        int pad4;
    };
}