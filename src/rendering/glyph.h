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
        vec2 texturePosition; // glyph texture position in texel units
        vec2 textureSize; //glyph texture size in texel units
        vec2 texelSize;
        int unit;
        float page;

        glyph(phi::image* image);

        ~glyph();
    };
}