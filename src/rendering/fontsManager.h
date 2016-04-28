#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "glyph.h"
#include "font.h"
#include "texturesManager.h"

namespace phi
{
    class fontsManager
    {
    private:
        static FT_Library _freeTypeLibrary;
        static bool _initialized;

    private:
        string _path;
        texturesManager* _texturesManager;
        map<std::tuple<string, uint>, font*> _fonts;
        map<glyph*, glyphNode*> _glyphAtlas;
        glyphNode* _glyphAtlasRoot;
        texture* _glyphAtlasTexture;
        map<std::tuple<const font*, ulong>, glyph*> _glyphCache;
        GLint _glyphAtlasSize;
        textureAddress _glyphAtlasTextureAddress;

    public:
        RENDERING_API fontsManager(string path, texturesManager* texturesManager);
        RENDERING_API ~fontsManager();
        RENDERING_API font* load(string name, uint size);
        RENDERING_API glyph* getGlyph(font* const font, const ulong& glyphChar);
        GLint getGlyphAtlasSize() const { return _glyphAtlasSize; }
        texture* getGlyphTexture() const { return _glyphAtlasTexture; }
        textureAddress getGlyphAtlasTextureAdress() const { return _glyphAtlasTextureAddress; }
    };
}