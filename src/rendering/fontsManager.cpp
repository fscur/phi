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
        _glyphAtlas(map<glyph*, glyphNode*>()),
        _glyphAtlasRoot(nullptr),
        _glyphAtlasTexture(nullptr),
        _glyphCache(map<std::tuple<const font*, ulong>, glyph*>()),
        _glyphAtlasSize(-1),
        _glyphAtlasTextureAddress(textureAddress())
    {
        assert(!_initialized);

        if (!_initialized)
        {
            _initialized = true;
            FT_Init_FreeType(&_freeTypeLibrary);
            font::FreeTypeLibrary = _freeTypeLibrary;
        }

        auto glyphAtlasSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glyphAtlasSize);

        _glyphAtlasSize = std::min(glyphAtlasSize, 1024);
        _glyphAtlasRoot = new glyphNode(rectangle(0, 0, _glyphAtlasSize, _glyphAtlasSize));

        auto maxLevels = texturesManager::getMaxLevels(_glyphAtlasSize, _glyphAtlasSize);

        auto layout = phi::textureContainerLayout();
        layout.w = static_cast<GLsizei>(_glyphAtlasSize);
        layout.h = static_cast<GLsizei>(_glyphAtlasSize);
        layout.levels = maxLevels;
        layout.internalFormat = GL_RGB8;
        layout.wrapMode = GL_CLAMP_TO_EDGE;
        layout.minFilter = GL_LINEAR;
        layout.magFilter = GL_LINEAR;

        _texturesManager->reserveContainer(layout, 1);

        _glyphAtlasTexture = new texture(
            _glyphAtlasSize,
            _glyphAtlasSize,
            GL_TEXTURE_2D,
            GL_RGB8,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            nullptr,
            GL_CLAMP_TO_EDGE,
            GL_LINEAR,
            GL_LINEAR,
            true);

        glError::check();

        _glyphAtlasTextureAddress = _texturesManager->add(_glyphAtlasTexture);
    }

    fontsManager::~fontsManager()
    {
        for (auto pair : _fonts)
            safeDelete(pair.second);
        
        FT_Done_FreeType(_freeTypeLibrary);
    }

    glyph* fontsManager::getGlyph(font* const font, const ulong& glyphChar)
    {
        std::tuple<phi::font*, ulong> key(font, glyphChar);

        if (_glyphCache.find(key) != _glyphCache.end())
            return _glyphCache[key];

        auto glyph = font->getGlyph(glyphChar);

        auto glyphNode = _glyphAtlasRoot->insert(glyph);
        _glyphAtlas[glyph] = glyphNode;

        auto x = glyphNode->rect.x;
        auto y = glyphNode->rect.y;
        auto w = static_cast<GLsizei>(glyph->bitmapWidth);
        auto h = static_cast<GLsizei>(glyph->bitmapHeight);
        auto r = 1.0f / (float)_glyphAtlasSize;

        glyph->texPos = vec2((float)x * r, (float)y * r);
        glyph->texSize = vec2((float)w * r, (float)h * r);
        glyph->texPage = _glyphAtlasTextureAddress.page;

        glActiveTexture(GL_TEXTURE0 + _glyphAtlasTextureAddress.unit);
        glError::check();

        glBindTexture(GL_TEXTURE_2D_ARRAY, _glyphAtlasTextureAddress.containerId);
        glError::check();

        glTextureSubImage3D(//GL_TEXTURE_2D_ARRAY,
            _glyphAtlasTextureAddress.containerId,
            0,
            x,
            y,
            0,
            w,
            h,
            1,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            glyph->data);

        glError::check();
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glError::check();

        _glyphCache[key] = glyph;

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