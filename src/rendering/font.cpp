#include <precompiled.h>
#include "font.h"
#include <core\exception.h>

namespace phi
{
    FT_Library font::FreeTypeLibrary = nullptr;

    font::font(const string& fileName, const uint size) :
        _size(size),
        _hinting(false),
        _horizontalScale(1.0f),
        _dpi(96.0f)
    {
        if (font::FreeTypeLibrary == nullptr)
            throw phi::exception("FreeType not initialized. Inititialized Font Manager? ;)");

        FT_New_Face(font::FreeTypeLibrary, fileName.c_str(), 0, &_fontFace);
        FT_Set_Char_Size(_fontFace, _size * 64, 0, static_cast<int>(_dpi * _horizontalScale), static_cast<int>(_dpi));

        FT_Matrix matrix =
        {
            static_cast<int>((1.0f / _horizontalScale) * 0x10000L),
            static_cast<int>(0 * 0x10000L),
            static_cast<int>(0 * 0x10000L),
            static_cast<int>(1 * 0x10000L)
        };

        FT_Set_Transform(_fontFace, &matrix, NULL);

        _hasKerning = static_cast<bool>(FT_HAS_KERNING(_fontFace));
        _ascender = static_cast<float>(_fontFace->size->metrics.ascender >> 6);
        _baseLine = static_cast<float>(_fontFace->size->metrics.descender >> 6);
        _lineHeight = static_cast<float>(_fontFace->size->metrics.height >> 6);
    }

    glyph* font::getGlyph(const ulong& glyphChar)
    {
        auto glyphIndex = FT_Get_Char_Index(_fontFace, glyphChar);
        return getGlyph(glyphIndex);
    }

    glyph* font::getGlyph(const uint& glyphIndex)
    {
        if (_glyphCache.find(glyphIndex) != _glyphCache.end())
            return _glyphCache[glyphIndex];

        FT_GlyphSlot glyphSlot = _fontFace->glyph;
        float primary = 0.33f;
        float secondary = 0.33f;
        float tertiary = 0.0f;
        float norm = 1.0f / (primary + 2.0f * secondary + 2.0f * tertiary);

        byte lcdWeights[5];
        lcdWeights[0] = (byte)(tertiary * norm * 255);
        lcdWeights[1] = (byte)(secondary * norm * 255);
        lcdWeights[2] = (byte)(primary * norm * 255);
        lcdWeights[3] = (byte)(secondary * norm * 255);
        lcdWeights[4] = (byte)(tertiary * norm * 255);

        FT_Library_SetLcdFilter(font::FreeTypeLibrary, FT_LCD_FILTER_LIGHT);
        FT_Library_SetLcdFilterWeights(font::FreeTypeLibrary, lcdWeights);
        int flags = FT_LOAD_RENDER | FT_LOAD_TARGET_LCD;

        if (_hinting)
            flags |= FT_LOAD_FORCE_AUTOHINT;
        else
            flags |= FT_LOAD_NO_AUTOHINT | FT_LOAD_NO_HINTING;

        FT_Load_Glyph(_fontFace, glyphIndex, flags);

        auto buffer = glyphSlot->bitmap.buffer;
        auto w = glyphSlot->bitmap.width / 3;
        auto h = glyphSlot->bitmap.rows;

        auto g = new glyph();
        g->index = glyphIndex;
        g->width = static_cast<float>(w);
        g->height = static_cast<float>(h);
        g->offsetX = static_cast<float>(glyphSlot->bitmap_left);
        g->offsetY = static_cast<float>(glyphSlot->bitmap_top);
        g->horiBearingX = static_cast<float>((glyphSlot->metrics.horiBearingX >> 6) / _horizontalScale);
        g->horiBearingY = static_cast<float>(glyphSlot->metrics.horiBearingY >> 6);
        g->horiAdvance = static_cast<float>((glyphSlot->metrics.horiAdvance >> 6) / _horizontalScale);
        g->vertBearingX = static_cast<float>(glyphSlot->metrics.vertBearingX >> 6);
        g->vertBearingY = static_cast<float>(glyphSlot->metrics.vertBearingY >> 6);
        g->vertAdvance = static_cast<float>(glyphSlot->metrics.vertAdvance >> 6);
        g->data = buffer;
        _glyphCache[glyphIndex] = g;

        return g;
    }

    ivec2 font::getKerning(glyph* firstGlyph, glyph* secondGlyph)
    {
        if (_hasKerning &&
            firstGlyph != nullptr &&
            secondGlyph != nullptr)
        {
            FT_Vector kern;
            FT_Get_Kerning(
                _fontFace,
                firstGlyph->index,
                secondGlyph->index,
                FT_KERNING_DEFAULT,
                &kern);

            return ivec2((kern.x >> 6) / _horizontalScale, kern.y >> 6);
        }

        return ivec2(0);
    }

    font::~font()
    {
        FT_Done_Face(_fontFace);
    }
}