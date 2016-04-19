#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "texture.h"
#include "glyphNode.h"
#include "glyph.h"

namespace phi
{
    class font
    {
    public:
        static FT_Library FreeTypeLibrary;

    private:
        map<uint, glyph*> _glyphCache;
        FT_Face _fontFace;
        const uint _size;
        float _dpi;
        float _horizontalScale;
        float _baseLine;
        float _ascender;
        float _lineHeight;
        bool _hasKerning;
        bool _hinting;

    public:
        RENDERING_API font(const string& fileName, const uint size);
        RENDERING_API ~font();
        glyph* getGlyph(const ulong& glyphChar);
        glyph* getGlyph(const uint& glyphIndex);
        RENDERING_API ivec2 getKerning(glyph* firstGlyph, glyph* secondGlyph);
        RENDERING_API float getLineHeight() const { return _lineHeight; }
    };
}