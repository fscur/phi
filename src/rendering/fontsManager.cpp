#include <precompiled.h>
#include "fontsManager.h"
#include <io\path.h>

namespace phi
{
    FT_Library fontsManager::_freeTypeLibrary = nullptr;
    font* fontsManager::defaultFont = nullptr;
    bool fontsManager::_initialized = false;
    string fontsManager::_path = "";
    textureLayout fontsManager::_glyphLayout;
    vec2 fontsManager::_texelSize;
    map<std::tuple<string, uint>, font*> fontsManager::_fonts;
    unordered_map<glyph*, glyphTextureData*> fontsManager::_glyphTextureDataCache;

    //TODO: see what happens when there is no space in the container...
    GLint fontsManager::_maxGlyphAtlasSize = 1024;

    void fontsManager::initialize(string path)
    {
        _path = path;

        assert(!_initialized);

        if (!_initialized)
        {
            _initialized = true;
            FT_Init_FreeType(&_freeTypeLibrary);
            font::FreeTypeLibrary = _freeTypeLibrary;
        }

        defaultFont = load("Consolas.ttf", 14);
        createGlyphLayout();
    }

    void fontsManager::release()
    {
        for (auto& pair : _fonts)
            safeDelete(pair.second);
        
        for (auto& pair : _glyphTextureDataCache)
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

        texturesManager::reserveContainer(sizeui(glyphAtlasSize, glyphAtlasSize, 1), _glyphLayout);

        auto inverseGlyphAtlasSize = 1.0f / static_cast<float>(glyphAtlasSize);
        _texelSize = vec2(inverseGlyphAtlasSize);
    }

    glyphTextureData* fontsManager::getGlyph(font* const font, const ulong& glyphChar)
    {
        auto glyph = font->getGlyph(glyphChar);

        if (contains(_glyphTextureDataCache, glyph))
            return _glyphTextureDataCache[glyph];

        auto glyphTexture = new texture(glyph->image, _glyphLayout, true, true);
        auto address = texturesManager::addAtlasTexture(glyphTexture);

        auto glyphTextureData = new phi::glyphTextureData(glyph);

        glyphTextureData->texturePosition = vec2((float)address.rect.x * _texelSize.x, (float)address.rect.y * _texelSize.y);
        glyphTextureData->textureSize = vec2((float)address.rect.w * _texelSize.x, (float)address.rect.h * _texelSize.y);
        glyphTextureData->unit = address.unit;
        glyphTextureData->page = address.page;
        glyphTextureData->texelSize = _texelSize;

        _glyphTextureDataCache[glyph] = glyphTextureData;

        return glyphTextureData;
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