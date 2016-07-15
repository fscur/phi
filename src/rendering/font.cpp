#include <precompiled.h>
#include "font.h"
#include <core\exception.h>

namespace phi
{
    FT_Library font::FreeTypeLibrary = nullptr;

    font::font(const string& fileName, const uint size) :
        _size(size),
        _baseDpi(96.0f),
        _dpi(300.0f),
        _dpiRatio(_baseDpi / _dpi),
        _horizontalScale(1.0f),
        _baseLine(0.0f),
        _ascender(0.0f),
        _lineHeight(0.0f),
        _spacing(2.0f),
        _hasKerning(false),
        _hinting(false)
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
        _ascender = static_cast<float>(_fontFace->size->metrics.ascender >> 6) * _dpiRatio;
        _baseLine = static_cast<float>(_fontFace->size->metrics.descender >> 6) * _dpiRatio;
        _lineHeight = static_cast<float>((_fontFace->size->metrics.ascender - _fontFace->size->metrics.descender) >> 6) * _dpiRatio;

        float primary = 0.33f;
        float secondary = 0.33f;
        float tertiary = 0.0f;
        float norm = 1.0f / (primary + 2.0f * secondary + 2.0f * tertiary);

        _lcdWeights[0] = (byte)(tertiary * norm * 255);
        _lcdWeights[1] = (byte)(secondary * norm * 255);
        _lcdWeights[2] = (byte)(primary * norm * 255);
        _lcdWeights[3] = (byte)(secondary * norm * 255);
        _lcdWeights[4] = (byte)(tertiary * norm * 255);

        FT_Library_SetLcdFilter(font::FreeTypeLibrary, FT_LCD_FILTER_LIGHT);

        _loadGlyphFlags = FT_LOAD_RENDER | FT_LOAD_TARGET_LCD;

        if (_hinting)
            _loadGlyphFlags |= FT_LOAD_FORCE_AUTOHINT;
        else
            _loadGlyphFlags |= FT_LOAD_NO_AUTOHINT | FT_LOAD_NO_HINTING;
    }
    
    font::~font()
    {
        for (auto pair : _glyphCache)
            safeDelete(pair.second);

        FT_Done_Face(_fontFace);
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

        FT_Library_SetLcdFilterWeights(font::FreeTypeLibrary, _lcdWeights);

        FT_Load_Glyph(_fontFace, glyphIndex, _loadGlyphFlags);

        auto w = glyphSlot->bitmap.width;
        auto h = glyphSlot->bitmap.rows;
        auto size = h * glyphSlot->bitmap.pitch;

        byte* buffer = new byte[size];
        memcpy(buffer, glyphSlot->bitmap.buffer, size);

        const auto freetypeLcdScaleFactor = 3;
        w /= freetypeLcdScaleFactor;

        auto glyphImage = new image(
            w,
            h,
            imageDataFormat::rgb,
            imageDataType::ubyte_dataType,
            buffer);

        auto glyph = new phi::glyph(glyphImage);
        glyph->index = glyphIndex;
        glyph->width = static_cast<float>(w) * _dpiRatio;
        glyph->height = static_cast<float>(h) * _dpiRatio;
        glyph->offsetX = static_cast<float>(glyphSlot->bitmap_left) * _dpiRatio;
        glyph->offsetY = static_cast<float>(glyphSlot->bitmap_top) * _dpiRatio;
        glyph->horiBearingX = static_cast<float>((glyphSlot->metrics.horiBearingX >> 6) / _horizontalScale) * _dpiRatio;
        glyph->horiBearingY = static_cast<float>(glyphSlot->metrics.horiBearingY >> 6) * _dpiRatio;
        glyph->horiAdvance = static_cast<float>((glyphSlot->metrics.horiAdvance >> 6) / _horizontalScale) * _dpiRatio;
        glyph->vertBearingX = static_cast<float>(glyphSlot->metrics.vertBearingX >> 6) * _dpiRatio;
        glyph->vertBearingY = static_cast<float>(glyphSlot->metrics.vertBearingY >> 6) * _dpiRatio;
        glyph->vertAdvance = static_cast<float>(glyphSlot->metrics.vertAdvance >> 6) * _dpiRatio;
        _glyphCache[glyphIndex] = glyph;

        return glyph;
    }

    vec2 font::getKerning(glyph* firstGlyph, glyph* secondGlyph)
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

            return vec2(((kern.x >> 6) * _dpiRatio) / _horizontalScale, (kern.y >> 6) * _dpiRatio);
        }

        return vec2(0);
    }

    vec2 font::measureText(const wstring& text)
    {
        if (text.empty())
            return vec2(0.0f);

        auto x = _spacing * 2.0f;

        glyph* previousGlyph = nullptr;
        size_t textLength = text.length();

        for (size_t i = 0; i < textLength; i++)
        {
            auto glyph = getGlyph((ulong)text[i]);
            auto kern = getKerning(previousGlyph, glyph);
            x += glyph->horiAdvance + kern.x;
        }

        return vec2(x , _spacing * 2.0f + _lineHeight);
    }
}