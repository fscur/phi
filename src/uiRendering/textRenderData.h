#pragma once
#include <phi.h>
#include <core\color.h>

#include <rendering\font.h>

namespace phi
{
    struct textRenderData
    {
        font* font;
        vec3 position;
        wstring text;
        color color;
    };
}