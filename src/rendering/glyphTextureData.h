#pragma once
#include <phi.h>

#include "glyph.h"

namespace phi
{
    struct glyphTextureData
    {
    public:
        glyphTextureData(glyph* glyph) :
            glyph(glyph)
        {
        }

        ~glyphTextureData()
        {
            safeDelete(glyph);
        }

        glyph* glyph;
        vec2 texturePosition; // glyph texture position in texel units
        vec2 textureSize; //glyph texture size in texel units
        vec2 texelSize;
        int unit;
        float page;
    };
}