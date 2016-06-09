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
        map<std::tuple<string, uint>, font*> _fonts;
        GLint _maxGlyphAtlasSize;
        textureLayout _glyphLayout;
        vec2 _texelSize;

    private:
        void createGlyphLayout();

    public:
        RENDERING_API fontsManager(string path);
        RENDERING_API ~fontsManager();
        RENDERING_API font* load(string name, uint size);
        RENDERING_API glyph* getGlyph(font* const font, const ulong& glyphChar);
    };
}