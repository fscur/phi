#pragma once
#include <phi.h>
#include <core\color.h>

#include "font.h"

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