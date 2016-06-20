#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\atlas.h>

#include "glyph.h"
#include "font.h"
#include "glyphTextureData.h"
#include "texturesManager.h"

namespace phi
{
    class fontsManager
    {
    public:
        RENDERING_API static font* defaultFont;

    private:
        static FT_Library _freeTypeLibrary;
        static bool _initialized;
        static string _path;
        static map<std::tuple<string, uint>, font*> _fonts;
        static GLint _maxGlyphAtlasSize;
        static textureLayout _glyphLayout;
        static vec2 _texelSize;

        static unordered_map<glyph*, glyphTextureData*> _glyphTextureDataCache;

    private:
        static void createGlyphLayout();

    public:
        RENDERING_API static void initialize(string path);
        RENDERING_API static void release();
        RENDERING_API static font* load(string name, uint size);
        RENDERING_API static glyphTextureData* getGlyph(font* const font, const ulong& glyphChar);
    };
}