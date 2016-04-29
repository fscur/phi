#pragma once
#include <phi.h>
#include <core\color.h>
#include <rendering\texture.h>

namespace phi
{
    /*struct glyphInfo
    {
        vec2 pos;
        float page;
        float shift;
        vec2 size;
        float pad0;
        float pad1;
        color color;
    };*/

    struct glyphInfo
    {
        color color;
        vec2 pos;
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
        int texUnit;
        float texPage;

        ~glyph();
    };
}