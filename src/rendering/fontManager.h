#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "glyph.h"
#include "font.h"
#include "texturesManager.h"

namespace phi
{
    class fontManager
    {
    private:
        static FT_Library _freeTypeLibrary;
        static bool _freeTypeInitialized;

    private:
        map<std::tuple<const font*, ulong>, glyph*> _glyphCache;

        map<glyph*, glyphNode*> _glyphAtlas;
        glyphNode* _glyphAtlasRoot;
        GLint _glyphAtlasSize;
        texture* _glyphAtlasTexture;
        texturesManager* _texturesManager;
        textureAddress _glyphAtlasTextureAddress;

    public:
        RENDERING_API fontManager(texturesManager* texturesManager);
        RENDERING_API ~fontManager();
        RENDERING_API glyph* getGlyph(font* const font, const ulong& glyphChar);
        RENDERING_API GLint getGlyphAtlasSize() const { return _glyphAtlasSize; }
        RENDERING_API texture* getGlyphTexture() const { return _glyphAtlasTexture; }
        RENDERING_API textureAddress getGlyphAtlasTextureAdress() const { return _glyphAtlasTextureAddress; }
    };
}