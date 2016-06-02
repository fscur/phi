#pragma once
#include <phi.h>
#include <core\color.h>
#include <rendering\texture.h>

namespace phi
{
    struct glyph
    {
        image* image;
        uint index;
        float width;
        float height;
        float offsetX;
        float offsetY;
        float horiBearingX;
        float horiBearingY;
        float horiAdvance;
        float vertBearingX;
        float vertBearingY;
        float vertAdvance;
        vec2 texPosition;
        vec2 texSize;
        int texUnit;
        float texPage;

        glyph(phi::image* image);

        ~glyph();
    };


}