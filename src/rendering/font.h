#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "texture.h"
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
        float _spacing;
        bool _hasKerning;
        bool _hinting;
        int _loadGlyphFlags;
        byte _lcdWeights[5];
    private:
        bool hasKerning();

    public:
        RENDERING_API font(const string& fileName, const uint size);
        RENDERING_API ~font();

        float getLineHeight() const { return _lineHeight; }
        float getBaseLine() const { return _baseLine; }
        float getAscender() const { return _ascender; }
        float getSpacing() const { return _spacing; }

        glyph* getGlyph(const uint& glyphIndex);
        glyph* getGlyph(const ulong& glyphChar);

        RENDERING_API vec2 getKerning(glyph* firstGlyph, glyph* secondGlyph);
        RENDERING_API vec2 measureText(const wstring& text);
    };
}