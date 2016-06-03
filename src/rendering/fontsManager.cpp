#include <precompiled.h>
#include "fontsManager.h"
#include <io\path.h>

namespace phi
{
    FT_Library fontsManager::_freeTypeLibrary = nullptr;
    bool fontsManager::_initialized = false;

    void fontsManager::createGlyphAtlasContainer()
    {
        auto glyphAtlasSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glyphAtlasSize);
        glyphAtlasSize = std::min(glyphAtlasSize, _maxGlyphAtlasSize);

        auto maxLevels = texturesManager::getMaxLevels(glyphAtlasSize, glyphAtlasSize);

        auto layout = phi::textureContainerLayout();
        layout.w = static_cast<GLsizei>(glyphAtlasSize);
        layout.h = static_cast<GLsizei>(glyphAtlasSize);
        layout.levels = maxLevels;
        layout.internalFormat = GL_RGB8;
        layout.dataFormat = GL_RGB;
        layout.dataType = GL_UNSIGNED_BYTE;
        layout.wrapMode = GL_CLAMP_TO_EDGE;
        layout.minFilter = GL_LINEAR;
        layout.magFilter = GL_LINEAR;

        _glyphAtlasContainer = _texturesManager->reserveContainer(layout, 1);
    }

    fontsManager::fontsManager(string path, texturesManager* texturesManager) :
        _path(path),
        _texturesManager(texturesManager),
        _fonts(map<std::tuple<string, uint>, font*>()),
        _maxGlyphAtlasSize(1024),
        _glyphAtlasContainer(nullptr)
    {
        assert(!_initialized);

        if (!_initialized)
        {
            _initialized = true;
            FT_Init_FreeType(&_freeTypeLibrary);
            font::FreeTypeLibrary = _freeTypeLibrary;
        }

        createGlyphAtlasContainer();
    }

    fontsManager::~fontsManager()
    {
        for (auto pair : _fonts)
            safeDelete(pair.second);
        
        FT_Done_FreeType(_freeTypeLibrary);
    }

    glyph* fontsManager::getGlyph(font* const font, const ulong& glyphChar)
    {
        auto glyph = font->getGlyph(glyphChar);

        auto texelSize = _glyphAtlasContainer->getTexelSize();
        textureAddress address;
        _glyphAtlasContainer->subImage(glyph->image, address);

        glyph->texPosition = vec2((float)address.rect.x * texelSize.x, (float)address.rect.y * texelSize.y);
        glyph->texSize = vec2((float)address.rect.w * texelSize.x, (float)address.rect.h * texelSize.y);
        glyph->texUnit = address.unit;
        glyph->texPage = address.page;

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