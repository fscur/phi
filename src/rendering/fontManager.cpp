#include <precompiled.h>
#include "fontManager.h"

namespace phi
{
    FT_Library fontManager::_freeTypeLibrary = nullptr;
    bool fontManager::_freeTypeInitialized = false;

    fontManager::fontManager(texturesManager* texturesManager) :
        _texturesManager(texturesManager)
    {
        if (!_freeTypeInitialized)
        {
            FT_Init_FreeType(&_freeTypeLibrary);
            font::FreeTypeLibrary = _freeTypeLibrary;

            _freeTypeInitialized = true;
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

    fontManager::~fontManager()
    {
        FT_Done_FreeType(_freeTypeLibrary);
    }

    glyph* fontManager::getGlyph(font* const font, const ulong& glyphChar)
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

        _glyphCache[key] = glyph;

        return glyph;
    }
}