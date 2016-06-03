#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "glyph.h"
#include "font.h"
#include "texturesManager.h"
#include <core\atlas.h>

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
        GLint _maxGlyphAtlasSize;
        textureContainer* _glyphAtlasContainer;

    private:
        void createGlyphAtlasContainer();

    public:
        RENDERING_API fontsManager(string path, texturesManager* texturesManager);
        RENDERING_API ~fontsManager();
        RENDERING_API font* load(string name, uint size);
        RENDERING_API glyph* getGlyph(font* const font, const ulong& glyphChar);
        textureContainer* getGlyphAtlasContainer() const { return _glyphAtlasContainer; }
    };
}