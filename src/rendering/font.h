#pragma once
#include <phi.h>
#include "rendering.h"
#include <core\resource.h>
#include "texture.h"

namespace phi
{
    class font
    {
    private:
        static const int MAX_WIDTH = 1024;

        int _size;
        int _texWidth;
        int _texHeight;
        int _baseLine;
        int _ascender;
        int _lineHeight;
        texture* _texture;

    public:
        struct
        {
            float ax; // advance.x
            float ay; // advance.y

            float bw; // bitmap.width
            float bh; // bitmap.height

            float bl; // bitmap_left
            float bt; // bitmap_top (distance from baseline to the top of the glyph)

            float tx; // x offset of glyph in texture coordinates
            float ty; // y offset of glyph in texture coordinates
        } c[128];

    public:
        RENDERING_API font(std::string name, std::string path, int size, FT_Library library);
        RENDERING_API ~font();

        RENDERING_API static font* fromFile(string fileName);//TODO: unimplemented method

        RENDERING_API int getTexWidth() { return _texWidth; }
        RENDERING_API int getTexHeight() { return _texHeight; }
        RENDERING_API int getBaseLine() { return _baseLine; }
        RENDERING_API int getAscender() { return _ascender; }
        RENDERING_API int getLineHeight() { return _lineHeight; }
        RENDERING_API texture* getTexture() { return _texture; }
    };
}