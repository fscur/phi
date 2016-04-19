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
            false);

        _glyphAtlasTextureAddress = _texturesManager->add(_glyphAtlasTexture);
    }

    fontsManager::~fontsManager()
    {
        FT_Done_FreeType(_freeTypeLibrary);
        
        for (auto pair : _fonts)
            safeDelete(pair.second);
    }

    glyph* fontsManager::getGlyph(font* const font, const ulong& glyphChar)
    {
        std::tuple<const phi::font*, ulong> key(font, glyphChar);

        if (_glyphCache.find(key) != _glyphCache.end())
            return _glyphCache[key];

        auto glyph = font->getGlyph(glyphChar);

        auto glyphNode = _glyphAtlasRoot->insert(glyph);
        _glyphAtlas[glyph] = glyphNode;

        auto x = glyphNode->rect.x;
        auto y = glyphNode->rect.y;
        auto w = static_cast<GLsizei>(glyph->width);
        auto h = static_cast<GLsizei>(glyph->height);
        auto r = 1.0f / (float)_glyphAtlasSize;

        glyph->texPos = vec2((float)x * r, (float)y * r);
        glyph->texSize = vec2((float)w * r, (float)h * r);
        glyph->texPage = _glyphAtlasTextureAddress.page;

        //  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureSubImage3D(
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

        /*glBindTexture(GL_TEXTURE_2D_ARRAY, _glyphAtlasTextureAddress.containerId);
        glError::check();

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glError::check();*/

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