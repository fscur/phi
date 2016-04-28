#pragma once
#include <phi.h>

namespace phi
{
    struct glyphInfo
    {
        vec2 pos;
        float page;
        float shift;
        vec2 size;
        float pad0;
        float pad1;
    };

    struct glyph
    {
        void* data;
        uint index;
        float width;
        float height;
        float bitmapWidth;
        float bitmapHeight;
        float offsetX;
        float offsetY;
        float horiBearingX;
        float horiBearingY;
        float horiAdvance;
        float vertBearingX;
        float vertBearingY;
        float vertAdvance;
        vec2 texPos;
        vec2 texSize;
        float texPage;

        ~glyph();
    };
}