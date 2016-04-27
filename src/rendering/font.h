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
        float _baseDpi;
        float _dpi;
        float _dpiRatio;
        float _horizontalScale;
        float _baseLine;
        float _ascender;
        float _lineHeight;
        bool _hasKerning;
        bool _hinting;
        int _loadGlyphFlags;
        byte _lcdWeights[5];

    public:
        RENDERING_API font(const string& fileName, const uint size);
        RENDERING_API ~font();
        glyph* getGlyph(const ulong& glyphChar);
        glyph* getGlyph(const uint& glyphIndex);
        RENDERING_API vec2 getKerning(glyph* firstGlyph, glyph* secondGlyph);
        RENDERING_API float getLineHeight() const { return _lineHeight; }
        RENDERING_API float getBaseLine() const { return _baseLine; }
        RENDERING_API float getAscender() const { return _ascender; }
        RENDERING_API vec2 measureText(const wstring& text);
    };
}