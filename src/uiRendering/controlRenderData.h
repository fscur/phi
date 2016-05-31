#pragma once
#include <core\color.h>

namespace phi
{
    struct controlRenderData
    {
        color backgroundColor;
        int backgroundTextureUnit;
        int pad0;
        int pad1;
        int pad2;
        float backgroundTexturePage;
        float pad3;
        float pad4;
        float pad5;
    };
}
