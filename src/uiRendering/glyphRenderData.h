#pragma once
#include <phi.h>

namespace phi
{
    struct glyphRenderData
    {
        color color;
        vec2 position;
        vec2 size;
        vec2 texelSize;
        float shift;
        float page;
        int unit;
        int pad2;
        int pad3;
        int pad4;
    };
}