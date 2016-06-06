#include <precompiled.h>
#include "fontsManager.h"
#include <io\path.h>

namespace phi
{
    FT_Library fontsManager::_freeTypeLibrary = nullptr;
    bool fontsManager::_initialized = false;

    fontsManager::fontsManager(string path, texturesManager* texturesManager) :
        _path(path),
        _texturesManager(texturesManager),
        _fonts(map<std::tuple<string, uint>, font*>()),
        _maxGlyphAtlasSize(1024) //TODO: see what happens when there is no space in the container...
    {
        assert(!_initialized);

        if (!_initialized)
        {
            _initialized = true;
            FT_Init_FreeType(&_freeTypeLibrary);
            font::FreeTypeLibrary = _freeTypeLibrary;
        }

        createGlyphLayout();
    }

    fontsManager::~fontsManager()
    {
        for (auto pair : _fonts)
            safeDelete(pair.second);
        
        FT_Done_FreeType(_freeTypeLibrary);
    }

    void fontsManager::createGlyphLayout()
    {
        auto glyphAtlasSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glyphAtlasSize);
        glyphAtlasSize = std::min(glyphAtlasSize, _maxGlyphAtlasSize);

        auto maxLevels = texturesManager::getMaxLevels(glyphAtlasSize, glyphAtlasSize);

        _glyphLayout = phi::textureLayout();
        _glyphLayout.dataFormat = GL_RGB;
        _glyphLayout.dataType = GL_UNSIGNED_BYTE;
        _glyphLayout.levels = maxLevels;
        _glyphLayout.internalFormat = GL_RGB8;
        _glyphLayout.wrapMode = GL_CLAMP_TO_EDGE;
        _glyphLayout.minFilter = GL_LINEAR;
        _glyphLayout.magFilter = GL_LINEAR;

        _texturesManager->reserveContainer(sizeui(glyphAtlasSize, glyphAtlasSize, 1), _glyphLayout);

        auto inverseGlyphAtlasSize = 1.0f / static_cast<float>(glyphAtlasSize);
        _texelSize = vec2(inverseGlyphAtlasSize);
    }

    glyph* fontsManager::getGlyph(font* const font, const ulong& glyphChar)
    {
        auto glyph = font->getGlyph(glyphChar);
        auto glyphTexture = new texture(glyph->image, _glyphLayout, true, true);

        textureAddress address = _texturesManager->get(glyphTexture);

        glyph->texturePosition = vec2((float)address.rect.x * _texelSize.x, (float)address.rect.y * _texelSize.y);
        glyph->textureSize = vec2((float)address.rect.w * _texelSize.x, (float)address.rect.h * _texelSize.y);
        glyph->unit = address.unit;
        glyph->page = address.page;
        glyph->texelSize = _texelSize;

        return glyph;
    }

    font* fontsManager::load(string name, uint size)
    {
        auto key = std::tuple<string, uint>(name, size);

        if (_fonts.find(key) != _fonts.end())
            return _fonts[key];

        string fileName = path::combine(_path, name);
        auto font = new phi::font(fileName, size);
        _fonts[key] = font;
        return font;
    }
}