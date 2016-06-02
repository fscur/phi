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
        _glyphAtlasContainer(nullptr),
        _glyphAtlasTexture(nullptr),
        _glyphAtlasSize(-1),
        _glyphAtlasTextureAddress(textureAddress()),
        _glyphAtlas(nullptr)
    {
        assert(!_initialized);

        if (!_initialized)
        {
            _initialized = true;
            FT_Init_FreeType(&_freeTypeLibrary);
            font::FreeTypeLibrary = _freeTypeLibrary;
        }

        auto tempGlyphAtlasSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tempGlyphAtlasSize);

        _glyphAtlasSize = std::min(tempGlyphAtlasSize, 1024);

        _glyphAtlas = new atlas(sizeui(_glyphAtlasSize, _glyphAtlasSize));

        auto maxLevels = texturesManager::getMaxLevels(_glyphAtlasSize, _glyphAtlasSize);

        auto layout = phi::textureContainerLayout();
        layout.w = static_cast<GLsizei>(_glyphAtlasSize);
        layout.h = static_cast<GLsizei>(_glyphAtlasSize);
        layout.levels = maxLevels;
        layout.internalFormat = GL_RGB8;
        layout.dataFormat = GL_RGB;
        layout.dataType = GL_UNSIGNED_BYTE;
        layout.wrapMode = GL_CLAMP_TO_EDGE;
        layout.minFilter = GL_LINEAR;
        layout.magFilter = GL_LINEAR;

        _glyphAtlasContainer = _texturesManager->reserveContainer(layout, 1);

        _glyphAtlasTexture = new texture(
            _glyphAtlasSize,
            _glyphAtlasSize,
            layout.dataFormat,
            layout.dataType,
            nullptr,
            GL_TEXTURE_2D,
            layout.internalFormat,
            layout.wrapMode,
            layout.minFilter,
            layout.magFilter,
            true);

        glError::check();

        _glyphAtlasTextureAddress = _texturesManager->get(_glyphAtlasTexture);
    }

    fontsManager::~fontsManager()
    {
        for (auto pair : _fonts)
            safeDelete(pair.second);
        
        FT_Done_FreeType(_freeTypeLibrary);

        safeDelete(_glyphAtlas);
        safeDelete(_glyphAtlasTexture);
    }

    glyph* fontsManager::getGlyph(font* const font, const ulong& glyphChar)
    {
        auto glyph = font->getGlyph(glyphChar);

        auto glyphRect = rectangle<uint>(
            0u,
            0u,
            glyph->image->w, 
            glyph->image->h);
        
        auto rect = _glyphAtlas->insert(new atlasItem(glyphRect, glyph))->rect;

        auto x = rect.x;
        auto y = rect.y;
        auto w = rect.w;
        auto h = rect.h;

        auto texelSize = 1.0f / (float)_glyphAtlasSize;

        glyph->texPosition = vec2((float)rect.x * texelSize, (float)rect.y * texelSize);
        glyph->texSize = vec2((float)rect.w * texelSize, (float)rect.h * texelSize);
        glyph->texUnit = _glyphAtlasTextureAddress.unit;
        glyph->texPage = _glyphAtlasTextureAddress.page;

        auto subDataRect = rectangle<GLint>(x,y,w,h);
        _glyphAtlasContainer->subData(_glyphAtlasTextureAddress.page, subDataRect, glyph->image->data);

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